# Raft Experiment Timelapse Pipeline

This repository provides tools to **calibrate a fisheye camera** and generate a **time-lapse video with tracked raft movement** from experimental image sequences.

## Contents

- `fisheye_calibrate.py` – Calibrates the fisheye lens using chessboard images.
- `generate_experiment_timelapse.py` – Creates a processed video pipeline from raw experimental images, with options for undistortion, cropping, raft tracking, and video generation.

---

## 1. Fisheye Camera Calibration

### `fisheye_calibrate.py`

This script uses a set of calibration images with a chessboard pattern to compute the camera's intrinsic matrix and distortion coefficients.

### Required Input

- A folder named `camera_calibration_images` (in the same directory as the script), containing images named like `calib_001.png`, `calib_002.png`, etc.
- Images must show a **9x6 chessboard** (inner corners) and each square should be **25 mm** by default (customizable via `SQUARE_SIZE`).
- Pattern used can be found here: https://github.com/opencv/opencv/blob/4.x/doc/pattern.png

### How to Run

```bash
python fisheye_calibrate.py
```

### Output

- A file named `fisheye_calibration.npz` containing:
  - `K`: Intrinsic matrix
  - `D`: Distortion coefficients

---

## 2. Raft Experiment Timelapse

### `generate_experiment_timelapse.py`

Processes an experimental image sequence into a time-lapse video with optional raft tracking overlays.

### Configuration Options

Edit the top of the script to configure:
- `EXPERIMENT_NAME`: Folder name under `../output/` containing image frames and setup info.
- `IMAGE_VERSION`: One of `raw`, `undistorted`, `cropped`, `tracked`.
- `SPEEDUP_FACTOR`: Multiplier for real-time speed.
- `NUM_RAFTS`, `RAFT_RADIUS_PX`, and ROI details for tracking settings.
- Cropping window via `X_CROP`, `Y_CROP`, `W_CROP`, `H_CROP`.

### Required Input Structure Example

```
output/
└── experiment_E2/
    ├── images/
    │   └── frame_0000.jpg, frame_0001.jpg, ...
    └── experiment_E2_setup_info.txt
        └── includes: timelapse_interval_ms = <interval>;
```

### How to Run

```bash
python generate_experiment_timelapse.py
```

The script will:
1. **Undistort** images using calibration data.
2. **Crop** to the region of interest.
3. **Track** rafts using Hough Circle Detection + Hungarian matching.
4. **Compile** frames into a video.
5. (Optional) Clean up intermediate images.

### Output

- Video saved to: `../videos/Experiment <Series>/<Stage>/experiment_E2_<stage>_<speedup>x.mp4`
- Intermediate images stored in folders like `undistorted/`, `cropped/`, `tracked/` (deleted if `DELETE_INTERMEDIATE=True`).

---

## Example Workflow

1. **Calibrate Camera:**

```bash
python fisheye_calibrate.py
```

2. **Process and Generate Video:**

```bash
python generate_experiment_timelapse.py
```

(Ensure `fisheye_calibration.npz` exists in the same directory.)

---

## Dependencies

- Python 3
- OpenCV (`cv2`)
- NumPy
- SciPy

Install with:

```bash
pip install opencv-python numpy scipy
```
