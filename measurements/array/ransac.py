import numpy as np
from matplotlib import pyplot as plt

from skimage import data
from skimage.util import img_as_float
from skimage.feature import (
    corner_harris,
    corner_subpix,
    corner_peaks,
    plot_matched_features,
)
from skimage.transform import warp, AffineTransform
from skimage.exposure import rescale_intensity
from skimage.color import rgb2gray
from skimage.measure import ransac

src = np.array([
    [-11.8746000000000, 40.1851000000000, -80.4784000000000],
    [98.7129000000000, 35.7098000000000, -53.5039000000000],
    [48.8988000000000, 36.3966000000000, -61.9201000000000],
    [37.5243000000000, 37.9183000000000, -69.8313000000000],
    [43.4612000000000, 39.2212000000000, -64.7588000000000],
    [42.7978000000000, 35.6045000000000, -65.9523000000000],
    [39.8843000000000, 36.9345000000000, -65.8156000000000],
    [46.9146000000000, 37.8525000000000, -65.5195000000000],
    [43.1108000000000, 48.0508000000000, -55.2958000000000],
    [43.0405000000000, 26.1392000000000, -76.1694000000000],
    [46.5186000000000, -18.0573000000000, -85.8480000000000],
    [41.2007000000000, 92.6439000000000, -48.0092000000000],
    [42.4720000000000, 34.2307000000000, -65.6476000000000],
    [43.5774000000000, 40.5690000000000, -65.8460000000000],
    [45.2608000000000, 37.6276000000000, -65.2204000000000],
    [40.8438000000000, 36.6684000000000, -66.9524000000000],
    [42.5695000000000, 42.8627000000000, -61.7172000000000],
    [43.6941000000000, 31.4729000000000, -69.8316000000000],
    [53.6441000000000, 40.7782000000000, -53.2793000000000],
    [32.1675000000000, 32.7256000000000, -77.8956000000000],
    [12.4651000000000, -0.34944600000000, 26.4616000000000],
    [12.0039000000000, 0.039398600000000, 25.7657000000000],
    [46.2223000000000, 37.4634000000000, -67.9388000000000],
    [46.4799000000000, 37.2288000000000, -67.8984000000000],
    [44.0727000000000, 37.7577000000000, -67.0156000000000],
    [44.1239000000000, 37.8716000000000, -66.7014000000000],
    [43.1449000000000, 40.8174000000000, -67.6945000000000],
    [42.9839000000000, 40.6840000000000, -67.4128000000000],
    [53.0073000000000, 45.3020000000000, -67.0704000000000],
    [52.9825000000000, 45.4793000000000, -67.1603000000000]
])
dst = np.array([
    [-65.0117270648218, 2.25909476287026, -8.28334746385761],
    [65.0117270648218, -2.25909476287026, 8.28334746385761],
    [6.19971313930650, -1.16016601312248, 4.25394204811576],
    [-6.19971313930650, 1.16016601312248, -4.25394204811576],
    [0.567758499067024, 2.19782437916568, 0.895409932252683],
    [-0.567758499067024, -2.19782437916568, -0.895409932252683],
    [-3.91708545874231, -0.186747452712259, -0.0760822955494388],
    [3.91708545874231, 0.186747452712259, 0.0760822955494388],
    [-0.0154517084316154, 12.7940145813792, 11.7278466995976],
    [0.0154517084316154, -12.7940145813792, -11.7278466995976],
    [4.78921458959024, -64.1222620050693, -26.3406802427463],
    [-4.78921458959024, 64.1222620050693, 26.3406802427463],
    [-0.493991376792133, -3.53047011747076, -0.194068040882624],
    [0.493991376792133, 3.53047011747076, 0.194068040882624],
    [2.18265556156548, 0.403006664145259, 0.857471827757866],
    [-2.18265556156548, -0.403006664145259, -0.857471827757866],
    [-0.888696067624312, 6.82785769602862, 4.88782837193372],
    [0.888696067624312, -6.82785769602862, -4.88782837193372],
    [12.0555033478493, 4.27151785287995, 13.2610536826343],
    [-12.0555033478493, -4.27151785287995, -13.2610536826343],
    [-30.8637153251813, -46.2955729877719, 107.087739537371],
    [-30.8637153251813, -46.2955729877719, 107.087739537371],
    [3.55609002413563, -0.381009645443103, -2.12056883473889],
    [3.55609002413563, -0.381009645443103, -2.12056883473889],
    [0.832162431989255, 0.802442345132496, -1.14467364833154],
    [0.832162431989255, 0.802442345132496, -1.14467364833154],
    [-0.300999326434028, 4.06349090685938, -2.23925256483497],
    [-0.300999326434028, 4.06349090685938, -2.23925256483497],
    [10.6267198944092, 9.80927990253154, -1.75873452797914],
    [10.6267198944092, 9.80927990253154, -1.75873452797914]
])

class AffineTransform3D(AffineTransform):
    def __init__(self):
        super().__init__(dimensionality=3)

model = AffineTransform(dimensionality=3)
model.estimate(src, dst)

print("Affine transform:")
print(model.params)

model_robust, inliers = ransac(
    (src, dst), AffineTransform3D, min_samples=4, residual_threshold=1, max_trials=1000
)
outliers = inliers == False
print("RANSAC Affine transform 3D:")
print(outliers)
print(model_robust.params)


if True:
    # generate synthetic checkerboard image and add gradient for the later matching
    checkerboard = img_as_float(data.checkerboard())
    img_orig = np.zeros(list(checkerboard.shape) + [3])
    img_orig[..., 0] = checkerboard
    gradient_r, gradient_c = np.mgrid[0: img_orig.shape[0], 0: img_orig.shape[1]] / float(
        img_orig.shape[0]
    )
    img_orig[..., 1] = gradient_r
    img_orig[..., 2] = gradient_c
    img_orig = rescale_intensity(img_orig)
    img_orig_gray = rgb2gray(img_orig)

    # warp synthetic image
    tform = AffineTransform(scale=(0.9, 0.9), rotation=0.2, translation=(20, -10))
    img_warped = warp(img_orig, tform.inverse, output_shape=(200, 200))
    img_warped_gray = rgb2gray(img_warped)

    # extract corners using Harris' corner measure
    coords_orig = corner_peaks(
        corner_harris(img_orig_gray), threshold_rel=0.001, min_distance=5
    )
    coords_warped = corner_peaks(
        corner_harris(img_warped_gray), threshold_rel=0.001, min_distance=5
    )

    # determine sub-pixel corner position
    coords_orig_subpix = corner_subpix(img_orig_gray, coords_orig, window_size=9)
    coords_warped_subpix = corner_subpix(img_warped_gray, coords_warped, window_size=9)


    def gaussian_weights(window_ext, sigma=1):
        y, x = np.mgrid[-window_ext: window_ext + 1, -window_ext: window_ext + 1]
        g = np.zeros(y.shape, dtype=np.double)
        g[:] = np.exp(-0.5 * (x ** 2 / sigma ** 2 + y ** 2 / sigma ** 2))
        g /= 2 * np.pi * sigma * sigma
        return g


    def match_corner(coord, window_ext=5):
        r, c = np.round(coord).astype(np.intp)
        window_orig = img_orig[
                      r - window_ext: r + window_ext + 1, c - window_ext: c + window_ext + 1, :
                      ]

        # weight pixels depending on distance to center pixel
        weights = gaussian_weights(window_ext, 3)
        weights = np.dstack((weights, weights, weights))

        # compute sum of squared differences to all corners in warped image
        SSDs = []
        for cr, cc in coords_warped:
            window_warped = img_warped[
                            cr - window_ext: cr + window_ext + 1,
                            cc - window_ext: cc + window_ext + 1,
                            :,
                            ]
            SSD = np.sum(weights * (window_orig - window_warped) ** 2)
            SSDs.append(SSD)

        # use corner with minimum SSD as correspondence
        min_idx = np.argmin(SSDs)
        return coords_warped_subpix[min_idx]


    # find correspondences using simple weighted sum of squared differences
    src = []
    dst = []
    for coord in coords_orig_subpix:
        src.append(coord)
        dst.append(match_corner(coord))
    src = np.array(src)
    dst = np.array(dst)

    # estimate affine transform model using all coordinates
    model = AffineTransform()
    model.estimate(src, dst)

    # robustly estimate affine transform model with RANSAC
    model_robust, inliers = ransac(
        (src, dst), AffineTransform, min_samples=3, residual_threshold=2, max_trials=100
    )
    outliers = inliers == False

    # compare "true" and estimated transform parameters
    print("Ground truth:")
    print(
        f'Scale: ({tform.scale[1]:.4f}, {tform.scale[0]:.4f}), '
        f'Translation: ({tform.translation[1]:.4f}, '
        f'{tform.translation[0]:.4f}), '
        f'Rotation: {-tform.rotation:.4f}'
    )
    print("Affine transform:")
    print(
        f'Scale: ({model.scale[0]:.4f}, {model.scale[1]:.4f}), '
        f'Translation: ({model.translation[0]:.4f}, '
        f'{model.translation[1]:.4f}), '
        f'Rotation: {model.rotation:.4f}'
    )
    print("RANSAC:")
    print(
        f'Scale: ({model_robust.scale[0]:.4f}, {model_robust.scale[1]:.4f}), '
        f'Translation: ({model_robust.translation[0]:.4f}, '
        f'{model_robust.translation[1]:.4f}), '
        f'Rotation: {model_robust.rotation:.4f}'
    )

    # visualize correspondence
    fig, ax = plt.subplots(nrows=2, ncols=1)

    plt.gray()

    inlier_idxs = np.nonzero(inliers)[0]
    plot_matched_features(
        img_orig_gray,
        img_warped_gray,
        keypoints0=src,
        keypoints1=dst,
        matches=np.column_stack((inlier_idxs, inlier_idxs)),
        ax=ax[0],
        matches_color='b',
    )
    ax[0].axis('off')
    ax[0].set_title('Correct correspondences')

    outlier_idxs = np.nonzero(outliers)[0]
    plot_matched_features(
        img_orig_gray,
        img_warped_gray,
        keypoints0=src,
        keypoints1=dst,
        matches=np.column_stack((outlier_idxs, outlier_idxs)),
        ax=ax[1],
        matches_color='r',
    )
    ax[1].axis('off')
    ax[1].set_title('Faulty correspondences')

    plt.show()
