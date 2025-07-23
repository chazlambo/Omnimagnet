import cv2
import numpy as np
import os
import sys
from glob import glob

def load_fisheye_calibration():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    calib_path = os.path.join(script_dir, "fisheye_calibration.npz")

    if not os.path.exists(calib_path):
        raise FileNotFoundError(f"Calibration file not found at {calib_path}")

    data = np.load(calib_path)
    K = data["K"]
    D = data["D"]
    return K, D

def undistort_images(experiment_name):
    # Setup paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.abspath(os.path.join(script_dir, "..", "output", experiment_name))
    output_path = os.path.join(script_dir, "undistorted", experiment_name)
    os.makedirs(output_path, exist_ok=True)

    # Load fisheye calibration
    K, D = load_fisheye_calibration()

    # Get image list
    image_paths = sorted(glob(os.path.join(input_path, "frame_*.jpg")))
    if not image_paths:
        print(f"No images found in {input_path}")
        return

    # Read sample image to get size
    sample = cv2.imread(image_paths[0])
    if sample is None:
        raise RuntimeError(f"Failed to read sample image from {image_paths[0]}")
    h, w = sample.shape[:2]

    # Undistort and save
    for img_path in image_paths:
        img = cv2.imread(img_path)
        if img is None:
            print(f"⚠️ Couldn't read {img_path}")
            continue
        undistorted = cv2.fisheye.undistortImage(img, K, D, None, K)
        out_path = os.path.join(output_path, os.path.basename(img_path))
        cv2.imwrite(out_path, undistorted)
        print(f"✅ Saved: {out_path}")

    print(f"\n🎉 All fisheye images undistorted to: {output_path}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python undistort_images.py <experiment_name>")
        sys.exit(1)

    undistort_images(sys.argv[1])
