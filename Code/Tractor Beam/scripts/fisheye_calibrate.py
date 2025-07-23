# fisheye_calibrate.py
import cv2
import numpy as np
import os
from glob import glob

CHECKERBOARD = (9, 6)  # inner corners (not squares)
SQUARE_SIZE = 25.0     # mm (adjust to match your print)

def run_fisheye_calibration(image_dir, output_file):
    objp = np.zeros((1, CHECKERBOARD[0] * CHECKERBOARD[1], 3), np.float32)
    objp[0, :, :2] = np.mgrid[0:CHECKERBOARD[0], 0:CHECKERBOARD[1]].T.reshape(-1, 2)
    objp *= SQUARE_SIZE

    objpoints = []
    imgpoints = []
    images = sorted(glob(os.path.join(image_dir, "calib_*.png")))

    for fname in images:
        img = cv2.imread(fname)
        gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

        ret, corners = cv2.findChessboardCorners(
            gray, CHECKERBOARD,
            cv2.CALIB_CB_ADAPTIVE_THRESH +
            cv2.CALIB_CB_FAST_CHECK +
            cv2.CALIB_CB_NORMALIZE_IMAGE
        )

        if ret:
            objpoints.append(objp)
            cv2.cornerSubPix(
                gray, corners, (3, 3), (-1, -1),
                criteria=(cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 30, 0.001)
            )
            imgpoints.append(corners)
        else:
            print(f"Corners not found in {fname}")

    if not objpoints:
        print("No valid calibration images found. Exiting.")
        return

    N_OK = len(objpoints)
    K = np.zeros((3, 3))
    D = np.zeros((4, 1))
    rvecs = [np.zeros((1, 1, 3), dtype=np.float64)] * N_OK
    tvecs = [np.zeros((1, 1, 3), dtype=np.float64)] * N_OK

    rms, _, _, _, _ = cv2.fisheye.calibrate(
        objpoints, imgpoints, gray.shape[::-1],
        K, D, rvecs, tvecs,
        cv2.fisheye.CALIB_RECOMPUTE_EXTRINSIC,
        (cv2.TERM_CRITERIA_EPS + cv2.TERM_CRITERIA_MAX_ITER, 100, 1e-6)
    )

    print(f"RMS error: {rms}")
    print(f"K = \n{K}")
    print(f"D = \n{D.T}")

    np.savez(output_file, K=K, D=D)
    print(f"Saved calibration to: {output_file}")

if __name__ == "__main__":
    script_dir = os.path.dirname(os.path.abspath(__file__))
    run_fisheye_calibration(
        os.path.join(script_dir, "camera_calibration_images"),
        os.path.join(script_dir, "fisheye_calibration.npz")
    )
