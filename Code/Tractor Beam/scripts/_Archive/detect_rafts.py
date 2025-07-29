import cv2
import numpy as np
import os
from glob import glob

# ====== USER CONFIGURATION ======
EXPERIMENT_NAME = "experiment_D1"
NUM_RAFTS = 2
RAFT_RADIUS_PX = 32
RADIUS_TOLERANCE = 4
ROI_TOP_LEFT = (70, 130)
ROI_BOTTOM_RIGHT = (720, 350)
DRAW_ROI_BOX = False
SMOOTHING_ALPHA = 0.4  # 0 = only history, 1 = no smoothing
# =================================

def detect_and_draw_rafts(experiment_name):
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_dir = os.path.join(script_dir, "cropped", experiment_name)
    output_dir = os.path.join(script_dir, "detected", experiment_name)
    os.makedirs(output_dir, exist_ok=True)

    image_paths = sorted(glob(os.path.join(input_dir, "frame_*_cropped.jpg")))
    if not image_paths:
        print(f"No images found in {input_dir}")
        return

    # Smoothing history buffer
    prev_centroids = None

    for img_path in image_paths:
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
        max_r = RAFT_RADIUS_PX + RADIUS_TOLERANCE

        circles = cv2.HoughCircles(
            blurred, cv2.HOUGH_GRADIENT, dp=1.2, minDist=RAFT_RADIUS_PX,
            param1=100, param2=30,
            minRadius=min_r, maxRadius=max_r
        )

        centroids = []

        if circles is not None:
            circles = np.uint16(np.around(circles[0]))
            circles = sorted(circles, key=lambda c: c[0])  # left-to-right sort

            for (x, y, r) in circles[:NUM_RAFTS]:
                centroids.append((x + x1, y + y1, r))

        # Apply smoothing if previous frame exists
        if prev_centroids and len(prev_centroids) == len(centroids):
            smoothed_centroids = []
            for i in range(len(centroids)):
                cx, cy, r = centroids[i]
                pcx, pcy, pr = prev_centroids[i]
                sx = int(SMOOTHING_ALPHA * cx + (1 - SMOOTHING_ALPHA) * pcx)
                sy = int(SMOOTHING_ALPHA * cy + (1 - SMOOTHING_ALPHA) * pcy)
                sr = int(SMOOTHING_ALPHA * r + (1 - SMOOTHING_ALPHA) * pr)
                smoothed_centroids.append((sx, sy, sr))
            centroids = smoothed_centroids

        # Draw on image
        for (x, y, r) in centroids:
            cv2.circle(img, (x, y), r, (0, 0, 255), 2)
            cv2.circle(img, (x, y), 3, (0, 0, 255), -1)

        if DRAW_ROI_BOX:
            cv2.rectangle(img, ROI_TOP_LEFT, ROI_BOTTOM_RIGHT, (0, 255, 0), 2)


        prev_centroids = centroids

        out_path = os.path.join(output_dir, os.path.basename(img_path))
        cv2.imwrite(out_path, img)
        print(f"Saved: {out_path}")

    print(f"\nRaft detection complete. Output saved to: {output_dir}")

if __name__ == "__main__":
    detect_and_draw_rafts(EXPERIMENT_NAME)
