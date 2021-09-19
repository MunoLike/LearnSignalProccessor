#include <opencv2/opencv.hpp>
#ifdef _DEBUG
#pragma comment(lib, "opencv_world453d.lib")
#else
#pragma comment(lib, "opencv_world453.lib")
#endif
#define NUM_OF_CLUSTERS 16
void k_means_method(cv::Mat&, cv::Mat&, long[]);
int main(void)
{
	int i, x, y;
	cv::Mat input_image = cv::imread("input.jpg", 0);
	//cv::imwrite("gray.png", input_image);
	cv::Mat clusters(input_image.rows, input_image.cols, CV_8UC1);
	cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);
	long means[NUM_OF_CLUSTERS];
	for (i = 0; i < NUM_OF_CLUSTERS; i++) {


		means[i] = 255 * (i + 1) / NUM_OF_CLUSTERS;
	}
	k_means_method(input_image, clusters, means);
	for (y = 0; y < input_image.rows; y++) {
		for (x = 0; x < input_image.cols; x++) {
			unsigned char c = clusters.ptr<unsigned char>(y)[x];
			output_image.ptr<unsigned char>(y)[x] = means[c];
		}
	}
	cv::imshow("Input Image", input_image);
	cv::imshow("Output Image", output_image);
	cv::waitKey();
	cv::imwrite("output.png", output_image);

	return 0;
}
void k_means_method(cv::Mat& image, cv::Mat& clusters, long means[])
{
	int i, x, y;
	int min_diff, diff;
	bool changed = false;
	int nums[NUM_OF_CLUSTERS];



	int count = 0;
	for (y = 0; y < image.rows; y++) {
		for (x = 0; x < image.cols; x++) {
			min_diff = image.ptr<unsigned char>(y)[x] -
				means[clusters.ptr<unsigned char>(y)[x]];
			min_diff *= min_diff;
			for (i = 0; i < NUM_OF_CLUSTERS; i++) {
				diff = image.ptr<unsigned char>(y)[x] - means[i];
				diff *= diff;
				if (diff < min_diff) {
					min_diff = diff;
					if (clusters.ptr<unsigned char>(y)[x] != i) {
						clusters.ptr<unsigned char>(y)[x] = i;
						changed = true;
						count++;
					}
				}
			}
		}
	}
	if (changed == false) {
		return;
	}
	for (i = 0; i < NUM_OF_CLUSTERS; i++) {
		means[i] = 0;
		nums[i] = 0;
	}
	for (y = 0; y < image.rows; y++) {
		for (x = 0; x < image.cols; x++) {
			means[clusters.ptr<unsigned char>(y)[x]] +=
				image.ptr<unsigned char>(y)[x];
			nums[clusters.ptr<unsigned char>(y)[x]]++;
		}
	}

	printf("%d pixels changed ... [ ", count);
	for (i = 0; i < NUM_OF_CLUSTERS; i++) {
		if (nums[i] != 0) {
			means[i] /= nums[i];
		}
		printf("%ld ", means[i]);
	}
	printf("]\n");
	k_means_method(image, clusters, means);
}