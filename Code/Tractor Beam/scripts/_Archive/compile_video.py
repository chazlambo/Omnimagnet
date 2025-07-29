import os
import cv2
from glob import glob

# ===== USER CONFIGURATION =====
experiment_name = "experiment_D1"       # Folder in output/
image_version = "tracked"               # Options: raw, undistorted, cropped, detected, tracked
speedup_factor = 30                     # E.g. 60 means 60x faster than real time
# =================================

def parse_timelapse_interval(setup_file_path):
    if not os.path.exists(setup_file_path):
        raise FileNotFoundError(f"Setup file not found: {setup_file_path}")

    with open(setup_file_path, "r") as f:
        for line in f:
            if "timelapse_interval_ms" in line:
                return int(line.split('=')[-1].strip().rstrip(';'))

    raise ValueError("timelapse_interval_ms not found in setup info file.")

def compile_images_to_video():
    script_dir = os.path.dirname(os.path.abspath(__file__))

    # Determine input image directory
    if image_version == "raw":
        image_dir = os.path.join(script_dir, "..", "output", experiment_name, "images")
        pattern = "frame_*.jpg"
    elif image_version == "undistorted":
        image_dir = os.path.join(script_dir, "undistorted", experiment_name)
        pattern = "frame_*_undistorted.jpg"
    elif image_version == "cropped":
        image_dir = os.path.join(script_dir, "cropped", experiment_name)
        pattern = "frame_*_cropped.jpg"
    elif image_version == "detected":
        image_dir = os.path.join(script_dir, "detected", experiment_name)
        pattern = "frame_*_cropped.jpg"
    elif image_version == "tracked":
        image_dir = os.path.join(script_dir, "tracked", experiment_name)
        pattern = "frame_*_tracked.jpg"
    else:
        raise ValueError(f"Invalid image version: {image_version}")

    image_paths = sorted(glob(os.path.join(image_dir, pattern)))
    if not image_paths:
        raise RuntimeError(f"No images found in {image_dir} matching pattern {pattern}")

    # Load first image for dimensions
    first_image = cv2.imread(image_paths[0])
    height, width = first_image.shape[:2]

    # Read timelapse interval
    setup_file_path = os.path.join(script_dir, "..", "output", experiment_name, f"{experiment_name}_setup_info.txt")
    interval_ms = parse_timelapse_interval(setup_file_path)
    real_fps = 1000 / interval_ms
    output_fps = real_fps * speedup_factor

    # Build video output directory
    videos_root = os.path.abspath(os.path.join(script_dir, "..", "videos"))
    experiment_video_dir = os.path.join(videos_root, experiment_name)
    os.makedirs(experiment_video_dir, exist_ok=True)

    # Final video output path
    video_filename = f"{experiment_name}_{image_version}_{speedup_factor}x.mp4"
    video_output_path = os.path.join(experiment_video_dir, video_filename)

    # Set up video writer
    fourcc = cv2.VideoWriter_fourcc(*'mp4v')
    video_writer = cv2.VideoWriter(video_output_path, fourcc, output_fps, (width, height))

    print(f"Creating video at {output_fps:.2f} fps from {len(image_paths)} frames...")

    for path in image_paths:
        frame = cv2.imread(path)
        if frame is None:
            print(f"Warning: Skipped unreadable frame {path}")
            continue
        video_writer.write(frame)

    video_writer.release()
    print(f"\nVideo saved to: {video_output_path}")

if __name__ == "__main__":
    compile_images_to_video()
