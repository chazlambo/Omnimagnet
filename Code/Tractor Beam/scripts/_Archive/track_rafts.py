import cv2
import numpy as np
import os
from glob import glob
from scipy.optimize import linear_sum_assignment
import shutil

# ====== USER CONFIGURATION ======
EXPERIMENT_NAME = "experiment_D1"
NUM_RAFTS = 2                          # Number of rafts in experiment
RAFT_RADIUS_PX = 32                    # Approximate expected raft radius in pixels
RADIUS_TOLERANCE = 4                   # Tolerance for raft radius in pixels
ROI_TOP_LEFT = (70, 130)               # Top left corner of region of interest rectangle
ROI_BOTTOM_RIGHT = (720, 350)          # Bottom right corner of region of interest rectangle
SMOOTHING_ALPHA = 0.6                  # 0 = only history, 1 = no smoothing
TRAIL_DURATION_SEC = 240               # seconds of trail to show
DRAW_RAFT_OUTLINE = True               # whether to draw the full circle outline
COLORS = [(255, 0, 0), (0, 255, 0)]    # BGR: Raft 0 = Blue, Raft 1 = Green
TRAIL_THICKNESS = 2                    # Thickness of the trail in pixels
# =================================

def euclidean(p1, p2):
    return np.linalg.norm(np.array(p1) - np.array(p2))

def parse_timelapse_interval(setup_file_path):
    if not os.path.exists(setup_file_path):
        raise FileNotFoundError(f"Setup file not found: {setup_file_path}")

    with open(setup_file_path, "r") as f:
        for line in f:
            if "timelapse_interval_ms" in line:
                return int(line.split('=')[-1].strip().rstrip(';'))

    raise ValueError("timelapse_interval_ms not found in setup info file.")

def track_and_draw_rafts(experiment_name):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_dir = os.path.join(script_dir, "cropped", experiment_name)
    output_dir = os.path.join(script_dir, "tracked", experiment_name)

    if os.path.exists(output_dir):
        shutil.rmtree(output_dir)
    os.makedirs(output_dir, exist_ok=True)

    # Determine frame interval and trail length in frames
    interval_ms = parse_timelapse_interval(script_dir, experiment_name)
    trail_frames = max(1, int(TRAIL_DURATION_SEC * 1000 / interval_ms))

    image_paths = sorted(glob(os.path.join(input_dir, "frame_*_cropped.jpg")))
    if not image_paths:
        print(f"No images found in {input_dir}")
        return

    prev_centroids = []
    prev_velocities = []
    raft_tracks = {i: [] for i in range(NUM_RAFTS)}

    for frame_idx, img_path in enumerate(image_paths):
        img = cv2.imread(img_path)
        if img is None:
            print(f"Couldn't read {img_path}")
            continue

        x1, y1 = ROI_TOP_LEFT
        x2, y2 = ROI_BOTTOM_RIGHT
        roi = img[y1:y2, x1:x2]
        gray = cv2.cvtColor(roi, cv2.COLOR_BGR2GRAY)
        blurred = cv2.medianBlur(gray, 5)

        min_r = RAFT_RADIUS_PX - RADIUS_TOLERANCE
        max_r = RAFT_RADIUS_PX + RAFT_RADIUS_PX

        circles = cv2.HoughCircles(
            blurred, cv2.HOUGH_GRADIENT, dp=1.2, minDist=RAFT_RADIUS_PX,
            param1=100, param2=30,
            minRadius=min_r, maxRadius=max_r
        )

        detections = []
        if circles is not None:
            circles = np.uint16(np.around(circles[0]))
            for (x, y, r) in circles[:NUM_RAFTS]:
                detections.append((x + x1, y + y1, r))

        if len(detections) < NUM_RAFTS:
            print(f"Frame {frame_idx:04d}: Incomplete detection, skipping.")
            continue

        if not prev_centroids:
            prev_centroids = detections
            prev_velocities = [(0, 0) for _ in range(NUM_RAFTS)]
        else:
            predicted = [
                (int(prev_centroids[i][0] + prev_velocities[i][0]),
                 int(prev_centroids[i][1] + prev_velocities[i][1]))
                for i in range(NUM_RAFTS)
            ]

            cost_matrix = np.zeros((NUM_RAFTS, len(detections)))
            for i in range(NUM_RAFTS):
                for j in range(len(detections)):
                    cost_matrix[i][j] = euclidean(predicted[i], detections[j][:2])

            row_ind, col_ind = linear_sum_assignment(cost_matrix)

            assigned = [None] * NUM_RAFTS
            for i, j in zip(row_ind, col_ind):
                assigned[i] = detections[j]

            new_centroids = []
            new_velocities = []
            for i in range(NUM_RAFTS):
                cx, cy, r = assigned[i]
                pcx, pcy, pr = prev_centroids[i]
                sx = int(SMOOTHING_ALPHA * cx + (1 - SMOOTHING_ALPHA) * pcx)
                sy = int(SMOOTHING_ALPHA * cy + (1 - SMOOTHING_ALPHA) * pcy)
                sr = int(SMOOTHING_ALPHA * r + (1 - SMOOTHING_ALPHA) * pr)
                new_centroids.append((sx, sy, sr))
                new_velocities.append((sx - pcx, sy - pcy))
            prev_centroids = new_centroids
            prev_velocities = new_velocities

        # Draw centroids and optional outlines
        for i, (x, y, r) in enumerate(prev_centroids):
            raft_tracks[i].append((x, y))
            if DRAW_RAFT_OUTLINE:
                cv2.circle(img, (x, y), r, COLORS[i], 2)
            cv2.circle(img, (x, y), 3, COLORS[i], -1)

        # Draw trails
        for i, trail in raft_tracks.items():
            for k in range(1, min(len(trail), trail_frames)):
                pt1 = trail[-k]
                pt2 = trail[-k - 1]
                fade = 1.0 - k / trail_frames
                color = tuple(int(c * fade) for c in COLORS[i])
                cv2.line(img, pt1, pt2, color, TRAIL_THICKNESS)

        out_filename = os.path.basename(img_path).replace("_cropped", "_tracked")
        out_path = os.path.join(output_dir, out_filename)
        cv2.imwrite(out_path, img)

    print(f"\nTracking complete. Tracked images saved to:\n{output_dir}")

if __name__ == "__main__":
    track_and_draw_rafts(EXPERIMENT_NAME)
