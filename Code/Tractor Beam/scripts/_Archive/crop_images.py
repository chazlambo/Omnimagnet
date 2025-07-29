import cv2
import os
from glob import glob

# ====== USER CONFIGURATION ======
EXPERIMENT_NAME = "experiment_D1"
X_CROP = 520    # X coordinate of top left corner of crop rectangle
Y_CROP = 740    # Y coordinate of top left corner of crop rectangle
W_CROP = 860    # Width of crop rectangle
H_CROP = 460    # Height of crop rectangle
# =================================

def crop_images(experiment_name):
    # Fixed crop region (x, y, width, height)
    x, y, w, h = X_CROP, Y_CROP, W_CROP, H_CROP

    # Setup paths
    script_dir = os.path.dirname(os.path.abspath(__file__))
    input_path = os.path.join(script_dir, "undistorted", experiment_name)
    output_path = os.path.join(script_dir, "cropped", experiment_name)
    os.makedirs(output_path, exist_ok=True)

    image_paths = sorted(glob(os.path.join(input_path, "frame_*_undistorted.jpg")))
    if not image_paths:
        print(f"No images found in {input_path}")
        return

    for img_path in image_paths:
        img = cv2.imread(img_path)
        if img is None:
            print(f"Couldn't read {img_path}")
            continue

        cropped = img[y:y+h, x:x+w]
        basename = os.path.basename(img_path).replace("_undistorted", "_cropped")
        out_path = os.path.join(output_path, basename)
        cv2.imwrite(out_path, cropped)
        print(f"Saved: {out_path}")

    print(f"\nAll images cropped to: {output_path}")

if __name__ == "__main__":
    crop_images(EXPERIMENT_NAME)
