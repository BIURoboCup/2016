/*
 * File :   ImageProcess.cpp
 * Authors : Guy & Alon
 *
 * Created on December 10, 2011
 */

#include "ImageProcess.h"
#include "Vision.h"

int16_t m_greenMinusRedThreshold;
int16_t m_greenMinusBlueThreshold;

int16_t m_blueMinusRedThreshold;
int16_t m_blueMinusGreenThreshold;

uint8_t m_orangeMinHue;
uint8_t m_orangeMaxHue;
uint8_t m_orangeMinSat;
uint8_t m_orangeMinVal;

uint8_t m_yellowMinHue;
uint8_t m_yellowMaxHue;
uint8_t m_yellowMinSat;
uint8_t m_yellowMinVal;

uint8_t m_whiteMaxSat;
uint8_t m_whitewMinVal;

uint8_t m_blackMaxVal;
/*
 * Description: Default constructor.
 * Parameters: None.
 * Return Value: None.
 */
ImageProcess::ImageProcess() {
}

/*
 * Description: Destructor.
 * Parameters: None.
 * Return Value: None.
 */
ImageProcess::~ImageProcess() {

}

/* Description: The function will do segmentation using the HSV color system.
 *             it will create simple threshold to the hue of orange, yellow and green.
 * Parameters: frame - the frame we are going to segment.
 * Return Value: None.*/
void ImageProcess::ImageHSVSegmentation(IplImage* hsvImage,
		IplImage* rgbImage) {
	int row;
	int col;

	uchar hue, sat, val;
	uchar* hsvImagePtr;

	uchar* rgbImagePtr;

	for (row = 0; row < hsvImage->height; row++) {

		for (col = 0; col < hsvImage->width; col++) {
			hsvImagePtr = (uchar*) (hsvImage->imageData
					+ row * hsvImage->widthStep + 3 * col);
			rgbImagePtr = (uchar*) (rgbImage->imageData
					+ row * rgbImage->widthStep + 3 * col);

			RGBFilter(rgbImagePtr, hsvImagePtr);

			hue = *(hsvImagePtr); // H component
			sat = *(hsvImagePtr + 1); // S component
			val = *(hsvImagePtr + 2); // V component

			if ((hue == BLUE_AVG_HUE)) {
				// Do nothing
			} else if ((hue == GREEN_AVG_HUE)) {
				if ((sat != MAX_SAT) || (val != MAX_VAL)) {
					hue = OTHER_HUE;
					val = MAX_VAL;
					sat = MAX_SAT;
				}
			}

			else if ((hue >= m_orangeMinHue) && (hue <= m_orangeMaxHue)
					&& (sat >= m_orangeMinSat) && (val >= m_orangeMinVal)) {
				hue = ORANGE_AVG_HUE;
				sat = MAX_SAT;
				val = MAX_VAL;
			} else if ((hue >= m_yellowMinHue) && (hue <= m_yellowMaxHue)
					&& (sat >= m_yellowMinSat) && (val >= m_yellowMinVal)) {
				hue = YELLOW_AVG_HUE;
				sat = MAX_SAT;
				val = MAX_VAL;
			} else if (val <= m_blackMaxVal) {
				val = MIN_VAL;
				sat = MIN_SAT;
			} else if (sat < m_whiteMaxSat && val >= m_whitewMinVal) {
				sat = MIN_SAT;
				val = MAX_VAL;
			}

			*(hsvImagePtr) = hue; // H component
			*(hsvImagePtr + 1) = sat; // S component
			*(hsvImagePtr + 2) = val; // V component
		}
	}
}

void ImageProcess::RGBFilter(uchar* rgbImagePtr, uchar* hsvImagePtr) {
	uchar hue = *(hsvImagePtr); // H component
	uchar sat = *(hsvImagePtr + 1); // S component
	uchar val = *(hsvImagePtr + 2); // V component

	uchar red = *(rgbImagePtr); // H component
	uchar green = *(rgbImagePtr + 1); // S component
	uchar blue = *(rgbImagePtr + 2); // V component

	int16_t greenThresholdOffsetByTilt = 0;

	if (Vision::GetInstance()->getCurrPicTilt() < MAX_TILT_FOR_GREEN_THRESHOLD_OFFSET) {
		greenThresholdOffsetByTilt = GREEN_TILT_THRESHOLD_OFFSET;
	}

#ifdef USE_OPENCV
	uchar temp = red; // H component
	red = blue;// S component
	blue = temp;// V component
#endif

	if (((int16_t) blue - (int16_t) red > m_blueMinusRedThreshold)
			&& ((int16_t) blue - (int16_t) green > m_blueMinusGreenThreshold)) {
		hue = BLUE_AVG_HUE;
		sat = MAX_SAT;
		val = MAX_VAL;
	}

	else if (((int16_t) green - (int16_t) red
			> m_greenMinusRedThreshold - greenThresholdOffsetByTilt)
			&& ((int16_t) green - (int16_t) blue
					> m_greenMinusBlueThreshold - greenThresholdOffsetByTilt)) {
		hue = GREEN_AVG_HUE;
		sat = MAX_SAT;
		val = MAX_VAL;
	}

	*(hsvImagePtr) = hue; // H component
	*(hsvImagePtr + 1) = sat; // S component
	*(hsvImagePtr + 2) = val; // V component

}

void ImageProcess::LoadIniSettings(minIni* iniFile) {
	LoadColorSettings(iniFile, "ORANGE");
	LoadColorSettings(iniFile, "YELLOW");
	LoadColorSettings(iniFile, "GREEN");
	LoadColorSettings(iniFile, "BLUE");
	LoadColorSettings(iniFile, "WHITE");
	LoadColorSettings(iniFile, "BLACK");
}

void ImageProcess::LoadColorSettings(minIni* iniFile,
		const std::string &section) {
	int value = -2;

	if (section.compare(ORANGE_STR) == 0) {
		if ((value = iniFile->geti(section, "min_hue", INVALID_VALUE))
				!= INVALID_VALUE)
			m_orangeMinHue = value;
		if ((value = iniFile->geti(section, "max_hue", INVALID_VALUE))
				!= INVALID_VALUE)
			m_orangeMaxHue = value;
		if ((value = iniFile->geti(section, "min_saturation", INVALID_VALUE))
				!= INVALID_VALUE)
			m_orangeMinSat = value;
		if ((value = iniFile->geti(section, "min_value", INVALID_VALUE))
				!= INVALID_VALUE)
			m_orangeMinVal = value;
	} else if (section.compare(YELLOW_STR) == 0) {
		if ((value = iniFile->geti(section, "min_hue", INVALID_VALUE))
				!= INVALID_VALUE)
			m_yellowMinHue = value;
		if ((value = iniFile->geti(section, "max_hue", INVALID_VALUE))
				!= INVALID_VALUE)
			m_yellowMaxHue = value;
		if ((value = iniFile->geti(section, "min_saturation", INVALID_VALUE))
				!= INVALID_VALUE)
			m_yellowMinSat = value;
		if ((value = iniFile->geti(section, "min_value", INVALID_VALUE))
				!= INVALID_VALUE)
			m_yellowMinVal = value;
	} else if (section.compare(GREEN_STR) == 0) {
		if ((value = iniFile->geti(section, "red_threshold", INVALID_VALUE))
				!= INVALID_VALUE)
			m_greenMinusRedThreshold = value;
		if ((value = iniFile->geti(section, "other_threshold", INVALID_VALUE))
				!= INVALID_VALUE)
			m_greenMinusBlueThreshold = value;
	} else if (section.compare(BLUE_STR) == 0) {
		if ((value = iniFile->geti(section, "red_threshold", INVALID_VALUE))
				!= INVALID_VALUE)
			m_blueMinusRedThreshold = value;
		if ((value = iniFile->geti(section, "other_threshold", INVALID_VALUE))
				!= INVALID_VALUE)
			m_blueMinusGreenThreshold = value;
	} else if (section.compare(WHITE_STR) == 0) {
		if ((value = iniFile->geti(section, "max_saturation", INVALID_VALUE))
				!= INVALID_VALUE)
			m_whiteMaxSat = value;
		if ((value = iniFile->geti(section, "min_value", INVALID_VALUE))
				!= INVALID_VALUE)
			m_whitewMinVal = value;
	} else if (section.compare(BLACK_STR) == 0) {
		if ((value = iniFile->geti(section, "max_value", INVALID_VALUE))
				!= INVALID_VALUE)
			m_blackMaxVal = value;
	}
}

void ImageProcess::LoadDefaultColorSettings() {
	m_orangeMinHue = DEFAULT_ORANGE_MIN_HUE;
	m_orangeMaxHue = DEFAULT_ORANGE_MAX_HUE;
	m_orangeMinSat = DEFAULT_ORANGE_MIN_SAT;
	m_orangeMinVal = DEFAULT_ORANGE_MIN_VAL;

	m_yellowMinHue = DEFAULT_YELLOW_MIN_HUE;
	m_yellowMaxHue = DEFAULT_YELLOW_MAX_HUE;
	m_yellowMinSat = DEFAULT_YELLOW_MIN_SAT;
	m_yellowMinVal = DEFAULT_YELLOW_MIN_VAL;

	m_greenMinusRedThreshold = DEFAULT_GREEN_MINUS_RED_THRESHOLD;
	m_greenMinusBlueThreshold = DEFAULT_GREEN_MINUS_BLUE_THRESHOLD;

	m_blueMinusRedThreshold = DEFAULT_BLUE_MINUS_RED_THRESHOLD;
	m_blueMinusGreenThreshold = DEFAULT_BLUE_MINUS_GREEN_THRESHOLD;

	m_whiteMaxSat = DEFAULT_WHITE_MAX_SAT;
	m_whitewMinVal = DEFAULT_WHITE_MIN_VAL;

	m_blackMaxVal = DEFAULT_BLACK_MAX_VAL;
}

