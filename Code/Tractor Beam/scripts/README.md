# Omnimagnet Tractor Beam Python Scripts

These scripts are used for post processing images taken from the omnimagnet tractor beam experiment

### Undistort Images ###
# Undistorts the fisheye lens effect from the experiment for the desired experiment name
python undistort_images.py test_exp_01

### Crop Images ###
# Crops the images to only include the water tank and magnets
python crop_images.py test_exp_01