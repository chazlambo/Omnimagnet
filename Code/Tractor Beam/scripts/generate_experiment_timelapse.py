import os
import cv2
import numpy as np
import shutil
from glob import glob
from scipy.optimize import linear_sum_assignment

# ====== EXPERIMENT CONFIGURATION ======
# Put all the names of experiments you want to process below
# IMPORTANT: Ensure that all experiments being processed simultaneously have the same number of rafts

EXPERIMENTS_TO_PROCESS = [
    "experiment_A2"
]

# 3 Raft Experiments
# EXPERIMENTS_TO_PROCESS = [
#     "experiment_A1",
#     "experiment_B2",
#     "experiment_B3"
# ]

# 2 Raft Experiments
# EXPERIMENTS_TO_PROCESS = [
#     "experiment_C1",
#     "experiment_C2",
#     "experiment_C3",
# ]


# ====== VIDEO CONFIGURATION ======
IMAGE_VERSION = "tracked"               # Options: raw, undistorted, cropped, tracked
SPEEDUP_FACTOR = 30                     # E.g. 60 means 60x faster than real time
DELETE_INTERMEDIATE = False             # Deletes all intermediary images generated to create timelapse

# ====== CROP CONFIGURATION ======
X_CROP = 520                            # X coordinate of top left corner of crop rectangle
Y_CROP = 740                            # Y coordinate of top left corner of crop rectangle
W_CROP = 860                            # Width of crop rectangle
H_CROP = 460                            # Height of crop rectangle

# ====== TRACKER CONFIGURATION ======
NUM_RAFTS = 3                          # Number of rafts in experiment
RAFT_RADIUS_PX = 32                    # Approximate expected raft radius in pixels
RADIUS_TOLERANCE = 4                   # Tolerance for raft radius in pixels
ROI_TOP_LEFT = (70, 130)               # Top left corner of region of interest rectangle
ROI_BOTTOM_RIGHT = (720, 350)          # Bottom right corner of region of interest rectangle
SMOOTHING_ALPHA = 0.5                  # 0 = only history, 1 = no smoothing
TRAIL_DURATION_SEC = 600               # seconds of trail to show
DRAW_RAFT_OUTLINE = True               # Whether to draw the full circle outline
OUTLINE_THICKNESS = 1                  # Thickness of the raft outline in pixels
TRAIL_THICKNESS = 2                    # Thickness of the trail in pixels

# ========== FUNCTIONS ==========

def load_fisheye_calibration():
    """Load camera calibration parameters from NPZ file."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    calib_path = os.path.join(script_dir, "fisheye_calibration.npz")

    data = np.load(calib_path)
    K = data["K"]
    D = data["D"]

    if K is None or D is None:
        raise ValueError("Failed to load camera calibration from fisheye_calibration.npz")
    return K, D

def parse_timelapse_interval(setup_file_path):
    """Parse timelapse interval from setup file."""
    with open(setup_file_path, "r") as f:
        for line in f:
            if "timelapse_interval_ms" in line:
                return int(line.split('=')[-1].strip().rstrip(';'))
    raise ValueError("timelapse_interval_ms not found in setup info file.")

def extract_experiment_series(experiment_name):
    """
    Extract the experiment series letter from experiment name.
    E.g., 'E1' -> 'E', 'E2' -> 'E', 'experiment_D1' -> 'D'
    """
    import re
    
    # Try to find a pattern like 'E1', 'E2', etc.
    match = re.search(r'([A-Za-z]+)\d+', experiment_name)
    if match:
        return match.group(1).upper()
    
    # Fallback: if no clear pattern, use the experiment name as-is
    clean_name = experiment_name.replace("experiment_", "").replace("_", "")
    if clean_name:
        return clean_name[0].upper()
    
    return "Unknown"

def generate_distinct_colors(num_colors):
    """
    Generate visually distinct colors for raft tracking.
    Returns BGR colors for OpenCV compatibility.
    """
    if num_colors <= 0:
        return []
    
    # High-contrast colors that work well against most backgrounds
    predefined_colors = [
        (255, 0, 0),    # Blue
        (0, 255, 0),    # Green  
        (0, 0, 255),    # Red
        (255, 255, 0),  # Cyan
        (255, 0, 255),  # Magenta
        (0, 255, 255),  # Yellow
        (128, 0, 255),  # Purple
        (0, 165, 255),  # Orange
        (203, 192, 255),# Pink
        (0, 128, 255),  # Dark Orange
        (147, 20, 255), # Deep Pink
        (0, 100, 0),    # Dark Green
    ]
    
    # Use predefined colors if we have enough
    if num_colors <= len(predefined_colors):
        return predefined_colors[:num_colors]
    
    # Generate additional colors using HSV for better distribution
    colors = predefined_colors.copy()
    import colorsys
    
    for i in range(len(predefined_colors), num_colors):
        # Distribute hues evenly across the color wheel
        hue = (i * 360 / num_colors) % 360
        saturation = 0.9 + (i % 2) * 0.1  # Alternate between 0.9 and 1.0
        value = 0.9 + (i % 3) * 0.1       # Cycle through brightness levels
        
        # Convert HSV to RGB then to BGR for OpenCV
        rgb = colorsys.hsv_to_rgb(hue/360, saturation, value)
        bgr = (int(rgb[2] * 255), int(rgb[1] * 255), int(rgb[0] * 255))
        colors.append(bgr)
    
    return colors

def experiment_exists(experiment_name):
    """Check if an experiment directory exists and contains images."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    experiment_path = os.path.abspath(os.path.join(script_dir, "..", "output", experiment_name))
    images_path = os.path.join(experiment_path, "images")
    
    if not os.path.exists(experiment_path):
        return False, f"Experiment directory not found: {experiment_path}"
    
    if not os.path.exists(images_path):
        return False, f"Images directory not found: {images_path}"
    
    # Check if there are any frame images
    frame_images = glob(os.path.join(images_path, "frame_*.jpg"))
    if not frame_images:
        return False, f"No frame images found in: {images_path}"
    
    return True, f"Found {len(frame_images)} images"

def undistort_images(experiment_name):
    """Remove fisheye distortion from raw images."""
    # Set up input and output paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.abspath(os.path.join(script_dir, "..", "output", experiment_name, "images"))
    output_path = os.path.join(script_dir, "undistorted", experiment_name)
    os.makedirs(output_path, exist_ok=True)

    # Load camera calibration parameters
    K, D = load_fisheye_calibration()

    # Get all frame images
    image_paths = sorted(glob(os.path.join(input_path, "frame_*.jpg")))
    if not image_paths:
        print(f"No images found in {input_path}")
        return

    # Create undistortion maps once for efficiency
    sample = cv2.imread(image_paths[0])
    h, w = sample.shape[:2]
    new_K = cv2.fisheye.estimateNewCameraMatrixForUndistortRectify(K, D, (w, h), np.eye(3), balance=0.0)
    map1, map2 = cv2.fisheye.initUndistortRectifyMap(K, D, np.eye(3), new_K, (w, h), cv2.CV_16SC2)

    # Process all images
    for img_path in image_paths:
        img = cv2.imread(img_path)
        if img is None:
            print(f"Couldn't read {img_path}")
            continue

        # Apply undistortion
        undistorted = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR)

        # Save with new filename
        basename = os.path.basename(img_path)
        name, ext = os.path.splitext(basename)
        new_filename = f"{name}_undistorted{ext}"
        out_path = os.path.join(output_path, new_filename)
        cv2.imwrite(out_path, undistorted)

    print(f"  - Undistorted {len(image_paths)} images")

def crop_images(experiment_name):
    """Crop images to focus on region of interest."""
    # Get crop dimensions from config
    x, y, w, h = X_CROP, Y_CROP, W_CROP, H_CROP

    # Set up paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, "undistorted", experiment_name)
    output_path = os.path.join(script_dir, "cropped", experiment_name)
    os.makedirs(output_path, exist_ok=True)

    # Get all undistorted images
    image_paths = sorted(glob(os.path.join(input_path, "frame_*_undistorted.jpg")))
    if not image_paths:
        print(f"No images found in {input_path}")
        return

    # Process each image
    for img_path in image_paths:
        img = cv2.imread(img_path)
        if img is None:
            print(f"Couldn't read {img_path}")
            continue

        # Apply crop
        cropped = img[y:y+h, x:x+w]
        
        # Save with new filename
        basename = os.path.basename(img_path).replace("_undistorted", "_cropped")
        out_path = os.path.join(output_path, basename)
        cv2.imwrite(out_path, cropped)

    print(f"  - Cropped {len(image_paths)} images")

def euclidean(p1, p2):
    """Calculate Euclidean distance between two points."""
    return np.linalg.norm(np.array(p1) - np.array(p2))

def get_timelapse_interval(script_dir, experiment_name):
    """Get timelapse interval from experiment setup file."""
    setup_path = os.path.join(script_dir, "..", "output", experiment_name, f"{experiment_name}_setup_info.txt")
    if not os.path.exists(setup_path):
        raise FileNotFoundError(f"Missing setup info file: {setup_path}")
    
    with open(setup_path, "r") as f:
        for line in f:
            if "timelapse_interval_ms" in line:
                return int(line.split('=')[-1].strip().rstrip(';'))

    raise ValueError("timelapse_interval_ms not found in setup info.")

def track_and_draw_rafts(experiment_name):
    """Track rafts across frames and draw visualization overlays."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_dir = os.path.join(script_dir, "cropped", experiment_name)
    output_dir = os.path.join(script_dir, "tracked", experiment_name)

    # Clean and create output directory
    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
    os.makedirs(output_dir, exist_ok=True)

    # Generate distinct colors for each raft
    colors = generate_distinct_colors(NUM_RAFTS)

    # Calculate how many frames to show in trail
    interval_ms = get_timelapse_interval(script_dir, experiment_name)
    trail_frames = max(1, int(TRAIL_DURATION_SEC * 1000 / interval_ms))

    # Get all cropped images
    image_paths = sorted(glob(os.path.join(input_dir, "frame_*_cropped.jpg")))
    if not image_paths:
        print(f"No images found in {input_dir}")
        return

    # Initialize tracking variables
    prev_centroids = []
    prev_velocities = []
    raft_tracks = {i: [] for i in range(NUM_RAFTS)}
    successful_frames = 0

    # Process each frame
    for frame_idx, img_path in enumerate(image_paths):
        img = cv2.imread(img_path)
        if img is None:
            continue

        # Extract region of interest for detection
        x1, y1 = ROI_TOP_LEFT
        x2, y2 = ROI_BOTTOM_RIGHT
        roi = img[y1:y2, x1:x2]
        gray = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
        blurred = cv2.medianBlur(gray, 5)

        # Set up circle detection parameters
        min_r = RAFT_RADIUS_PX - RADIUS_TOLERANCE
        max_r = RAFT_RADIUS_PX + RADIUS_TOLERANCE

        # Detect circular objects (rafts)
        circles = cv2.HoughCircles(
            blurred, cv2.HOUGH_GRADIENT, dp=1.2, minDist=RAFT_RADIUS_PX,
            param1=100, param2=30,
            minRadius=min_r, maxRadius=max_r
        )

        # Convert detections to full image coordinates
        detections = []
        if circles is not None:
            circles = np.uint16(np.around(circles[0]))
            for (x, y, r) in circles[:NUM_RAFTS]:
                detections.append((x + x1, y + y1, r))

        # Skip frame if we don't detect all rafts
        if len(detections) < NUM_RAFTS:
            continue

        # Handle first frame or tracking assignment
        if not prev_centroids:
            # First frame - just use detections as-is
            prev_centroids = detections
            prev_velocities = [(0, 0) for _ in range(NUM_RAFTS)]
        else:
            # Predict positions based on previous velocity
            predicted = [
                (int(prev_centroids[i][0] + prev_velocities[i][0]),
                 int(prev_centroids[i][1] + prev_velocities[i][1]))
                for i in range(NUM_RAFTS)
            ]

            # Create cost matrix for Hungarian algorithm
            cost_matrix = np.zeros((NUM_RAFTS, len(detections)))
            for i in range(NUM_RAFTS):
                for j in range(len(detections)):
                    cost_matrix[i][j] = euclidean(predicted[i], detections[j][:2])

            # Assign detections to rafts to minimize total distance
            row_ind, col_ind = linear_sum_assignment(cost_matrix)

            # Apply assignments
            assigned = [None] * NUM_RAFTS
            for i, j in zip(row_ind, col_ind):
                assigned[i] = detections[j]

            # Update positions with smoothing
            new_centroids = []
            new_velocities = []
            for i in range(NUM_RAFTS):
                if assigned[i] is None:
                    # Assignment failed - keep previous position
                    new_centroids.append(prev_centroids[i])
                    new_velocities.append(prev_velocities[i])
                    continue
                    
                # Apply temporal smoothing
                cx, cy, r = assigned[i]
                pcx, pcy, pr = prev_centroids[i]
                sx = int(SMOOTHING_ALPHA * cx + (1 - SMOOTHING_ALPHA) * pcx)
                sy = int(SMOOTHING_ALPHA * cy + (1 - SMOOTHING_ALPHA) * pcy)
                sr = int(SMOOTHING_ALPHA * r + (1 - SMOOTHING_ALPHA) * pr)
                new_centroids.append((sx, sy, sr))
                new_velocities.append((sx - pcx, sy - pcy))
                
            prev_centroids = new_centroids
            prev_velocities = new_velocities

        # Draw raft markers
        for i, (x, y, r) in enumerate(prev_centroids):
            # Add position to trail history
            raft_tracks[i].append((x, y))
            
            # Draw raft outline if enabled
            if DRAW_RAFT_OUTLINE:
                cv2.circle(img, (x, y), r, colors[i], OUTLINE_THICKNESS)
            
            # Draw center point
            cv2.circle(img, (x, y), 3, colors[i], -1)

        # Draw trails with fading effect
        for i, trail in raft_tracks.items():
            for k in range(1, min(len(trail), trail_frames)):
                pt1 = trail[-k]
                pt2 = trail[-k - 1]
                # Calculate fade based on age
                fade = 1.0 - k / trail_frames
                color = tuple(int(c * fade) for c in colors[i])
                cv2.line(img, pt1, pt2, color, TRAIL_THICKNESS)

        # Save processed frame
        out_filename = os.path.basename(img_path).replace("_cropped", "_tracked")
        out_path = os.path.join(output_dir, out_filename)
        cv2.imwrite(out_path, img)
        successful_frames += 1

    print(f"  - Tracked {successful_frames} frames (from {len(image_paths)} total)")

def compile_images_to_video(experiment_name):
    """Compile processed images into a video file."""
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Determine input directory and file pattern based on processing stage
    if IMAGE_VERSION == "raw":
        image_dir = os.path.join(script_dir, "..", "output", experiment_name, "images")
        pattern = "frame_*.jpg"
    elif IMAGE_VERSION == "undistorted":
        image_dir = os.path.join(script_dir, "undistorted", experiment_name)
        pattern = "frame_*_undistorted.jpg"
    elif IMAGE_VERSION == "cropped":
        image_dir = os.path.join(script_dir, "cropped", experiment_name)
        pattern = "frame_*_cropped.jpg"
    elif IMAGE_VERSION == "detected":
        image_dir = os.path.join(script_dir, "detected", experiment_name)
        pattern = "frame_*_detected.jpg"
    elif IMAGE_VERSION == "tracked":
        image_dir = os.path.join(script_dir, "tracked", experiment_name)
        pattern = "frame_*_tracked.jpg"
    else:
        raise ValueError(f"Invalid image version: {IMAGE_VERSION}")

    # Get all matching images
    image_paths = sorted(glob(os.path.join(image_dir, pattern)))
    if not image_paths:
        print(f"  - No images found for video compilation")
        return False

    # Get image dimensions from first frame
    first_image = cv2.imread(image_paths[0])
    if first_image is None:
        print(f"  - Could not load first image for video compilation")
        return False
    height, width = first_image.shape[:2]

    # Calculate video frame rate
    setup_file_path = os.path.join(script_dir, "..", "output", experiment_name, f"{experiment_name}_setup_info.txt")
    try:
        interval_ms = parse_timelapse_interval(setup_file_path)
        real_fps = 1000 / interval_ms
        output_fps = real_fps * SPEEDUP_FACTOR
    except (FileNotFoundError, ValueError) as e:
        print(f"  - Error getting timelapse interval: {e}")
        return False

    # Create output directory structure
    experiment_series = extract_experiment_series(experiment_name)
    videos_root = os.path.abspath(os.path.join(script_dir, "..", "videos"))
    series_dir = os.path.join(videos_root, f"Experiment {experiment_series}")
    experiment_video_dir = os.path.join(series_dir, IMAGE_VERSION.capitalize())
    os.makedirs(experiment_video_dir, exist_ok=True)

    # Set up output video path
    video_filename = f"{experiment_name}_{IMAGE_VERSION}_{SPEEDUP_FACTOR}x.mp4"
    video_output_path = os.path.join(experiment_video_dir, video_filename)

    # Initialize video writer
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video_writer = cv2.VideoWriter(video_output_path, fourcc, output_fps, (width, height))
    
    if not video_writer.isOpened():
        print(f"  - Failed to initialize video writer")
        return False

    # Write all frames to video
    frames_written = 0
    for path in image_paths:
        frame = cv2.imread(path)
        if frame is None:
            continue
        video_writer.write(frame)
        frames_written += 1

    video_writer.release()
    print(f"  - Created video: {video_filename} ({frames_written} frames, {output_fps:.1f} fps)")
    return True

def clean_intermediates(experiment_name):
    """Remove intermediate processing folders to save disk space."""
    script_dir = os.path.dirname(os.path.abspath(__file__))
    cleaned_folders = []
    for folder in ["undistorted", "cropped", "tracked"]:
        folder_path = os.path.join(script_dir, folder, experiment_name)
        if os.path.exists(folder_path):
            shutil.rmtree(folder_path)
            cleaned_folders.append(folder)
    
    if cleaned_folders:
        print(f"  - Cleaned intermediate folders: {', '.join(cleaned_folders)}")

def process_single_experiment(experiment_name):
    """Process a single experiment through the pipeline."""
    print(f"\n{'='*60}")
    print(f"Processing: {experiment_name}")
    print(f"{'='*60}")
    
    # Check if experiment exists
    exists, message = experiment_exists(experiment_name)
    if not exists:
        print(f"SKIPPED: {message}")
        return False
    
    print(f"{message}")
    
    try:
        # Define processing pipeline based on desired output
        stages = {
            "raw": [],
            "undistorted": [undistort_images],
            "cropped": [undistort_images, crop_images],
            "tracked": [undistort_images, crop_images, track_and_draw_rafts]
        }

        if IMAGE_VERSION not in stages:
            print(f"ERROR: Unsupported IMAGE_VERSION: {IMAGE_VERSION}")
            return False

        # Execute required processing stages
        for i, stage in enumerate(stages[IMAGE_VERSION], start=1):
            stage_name = stage.__name__.replace('_', ' ').title()
            print(f"[{i}/{len(stages[IMAGE_VERSION])}] {stage_name}...")
            stage(experiment_name)

        # Create final video output
        print(f"[Final] Compiling video...")
        success = compile_images_to_video(experiment_name)
        
        if not success:
            print(f"ERROR: Failed to create video")
            return False

        # Clean up intermediate files if requested
        if DELETE_INTERMEDIATE and stages[IMAGE_VERSION]:
            clean_intermediates(experiment_name)

        print(f"SUCCESS: {experiment_name} processing complete")
        return True
        
    except Exception as e:
        print(f"ERROR: Failed to process {experiment_name}: {str(e)}")
        return False

def main():
    """Main function to process multiple experiments."""
    print("=== Multi-Experiment Raft Timelapse Pipeline ===")
    print(f"Image Version: {IMAGE_VERSION}")
    print(f"Speedup Factor: {SPEEDUP_FACTOR}x")
    print(f"Delete Intermediates: {DELETE_INTERMEDIATE}")
    print(f"Experiments to process: {len(EXPERIMENTS_TO_PROCESS)}")
    
    # Process each experiment
    successful = 0
    failed = 0
    
    for experiment in EXPERIMENTS_TO_PROCESS:
        if process_single_experiment(experiment):
            successful += 1
        else:
            failed += 1
    
    # Print summary
    print(f"\n{'='*60}")
    print("PROCESSING SUMMARY")
    print(f"{'='*60}")
    print(f"Successful: {successful}")
    print(f"Failed: {failed}")
    print(f"Total: {successful + failed}")
    
    if failed > 0:
        print(f"\n {failed} experiment(s) failed to process. Check the output above for details.")
    else:
        print(f"\n All experiments processed successfully!")

if __name__ == "__main__":
    main()