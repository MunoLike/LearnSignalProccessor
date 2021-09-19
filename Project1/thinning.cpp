

#include "thinning.h"
#include "grayscale.h"


static void get_neighbors(cv::Mat&, int, int, int[]);
static void get_neighbors(cv::Mat& image, int x, int y, int neighbors[]) {
	int i;
	neighbors[0] = read_density(image, x + 1, y);
	neighbors[1] = read_density(image, x + 1, y - 1);
	neighbors[2] = read_density(image, x, y - 1);
	neighbors[3] = read_density(image, x - 1, y - 1);
	neighbors[4] = read_density(image, x - 1, y);
	neighbors[5] = read_density(image, x - 1, y + 1);
	neighbors[6] = read_density(image, x, y + 1);
	neighbors[7] = read_density(image, x + 1, y + 1);
	for (i = 0; i < 8; i++) {
		switch (neighbors[i]) {
		case 255:
			neighbors[i] = 1;
			break;
		case 0:
			neighbors[i] = 0;
			break;
		case 1:
			neighbors[i] = -1;
			break;
		}
	}
}

void hilditch_method(cv::Mat& image) {
	int i, j, x, y;
	int neighbors[8];
	int sum;
	bool finished;
	while (true) {
		finished = true;
		for (y = 0; y < image.rows; y++) {
			for (x = 0; x < image.cols; x++) {
				if (read_density(image, x, y) == 0) {
					continue;
				}
				get_neighbors(image, x, y, neighbors);
				sum = 0;
				for (i = 0; i < 4; i++) {
					sum += 1 - neighbors[2 * i] * neighbors[2 * i];
				}
				if (sum < 1) {
					continue;
				}
				sum = 0;
				for (i = 0; i < 8; i++) {
					sum += neighbors[i] * neighbors[i];
				}
				if (sum < 2) {
					continue;
				}
				sum = 0;
				for (i = 0; i < 8; i++) {
					sum += ((neighbors[i] == 1) ? 1 : 0);
				}
				if (sum < 1) {
					continue;
				}
				sum = 0;
				for (i = 0; i < 4; i++) {
					sum += neighbors[2 * i] * neighbors[2 * i] *
						(1 - neighbors[2 * i + 1] * neighbors[2 * i + 1] *
							neighbors[(i == 3) ? 0 : (2 * i + 2)] *
							neighbors[(i == 3) ? 0 : (2 * i + 2)]);

				}

				if (sum != 1) {
					continue;
				}
				for (i = 0; i < 8; i++) {
					if (neighbors[i] != -1) {
						continue;
					}
					neighbors[i] = 0;
					sum = 0;
					for (j = 0; j < 4; j++) {
						sum += neighbors[2 * j] * neighbors[2 * j] *
							(1 - neighbors[2 * j + 1] * neighbors[2 * j + 1] *
								neighbors[(j == 3) ? 0 : (2 * j + 2)] *
								neighbors[(j == 3) ? 0 : (2 * j + 2)]);

					}
					if (sum == 1) {
						neighbors[i] = -1;
					}
					else {
						break;
					}
				}
				if (sum != 1) {
					continue;
				}
				write_density(image, x, y, 1);
				finished = false;
			}
		}
		if (finished == true) {
			return;
		}
		for (y = 0; y < image.rows; y++) {
			for (x = 0; x < image.cols; x++) {
				if (read_density(image, x, y) == 1) {
					write_density(image, x, y, 0);
				}
			}
		}
	}
}