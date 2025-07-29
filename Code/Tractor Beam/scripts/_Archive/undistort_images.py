# undistort_images.py
import cv2
import numpy as np
import os
from glob import glob

# ====== USER CONFIGURATION ======
EXPERIMENT_NAME = "experiment_D1"
# =================================

def load_fisheye_calibration():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    calib_path = os.path.join(script_dir, "fisheye_calibration.npz")

    data = np.load(calib_path)
    K = data["K"]
    D = data["D"]

    if K is None or D is None:
        raise ValueError("Failed to load camera calibration from fisheye_calibration.npz")
    return K, D

def undistort_images(experiment_name):
    # Paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.abspath(os.path.join(script_dir, "..", "output", experiment_name, "images"))
    output_path = os.path.join(script_dir, "undistorted", experiment_name)
    os.makedirs(output_path, exist_ok=True)

    # Load calibration
    K, D = load_fisheye_calibration()

    # Collect images
    image_paths = sorted(glob(os.path.join(input_path, "frame_*.jpg")))
    if not image_paths:
        print(f"No images found in {input_path}")
        return

    # Undistortion maps
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

        undistorted = cv2.remap(img, map1, map2, interpolation=cv2.INTER_LINEAR)

        basename = os.path.basename(img_path)
        name, ext = os.path.splitext(basename)
        new_filename = f"{name}_undistorted{ext}"

        out_path = os.path.join(output_path, new_filename)
        cv2.imwrite(out_path, undistorted)
        print(f"Saved: {out_path}")

    print(f"\nAll images undistorted to: {output_path}")

if __name__ == "__main__":
    undistort_images(EXPERIMENT_NAME)
