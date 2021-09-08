#include <opencv2/opencv.hpp>
#ifdef _DEBUG
#pragma comment(lib, "opencv_world453d.lib")
#else
#pragma comment(lib, "opencv_world453.lib")
#endif
unsigned char get_density(cv::Mat&, int, int);
void get_neighbors(cv::Mat&, cv::Point, unsigned char[]);
cv::Point get_tracking_point(cv::Point, int);
cv::Point find_start_point(cv::Mat&, cv::Mat&, cv::Point);
void track_boundary(cv::Mat&, cv::Mat&, cv::Point, cv::Point, int);


int main(void)
{
	int x, y;



	cv::Point p;
	cv::Mat input_image = cv::imread("input.png", 0);
	cv::Mat output_image(input_image.rows, input_image.cols, CV_8UC1);
	for (y = 0; y < output_image.rows; y++) {
		for (x = 0; x < output_image.cols; x++) {
			output_image.ptr<unsigned char>(y)[x] = 0;
		}
	}
	x = 0; y = 0;
	p.x = x; p.y = y;
	while (y < input_image.rows) {
		p = find_start_point(input_image, output_image, p);
		if (p.x == -1 && p.y == -1) {
			x = 0; y++;
			p.x = x; p.y = y;
			continue;
		}
		output_image.ptr<unsigned char>(p.y)[p.x] = 255;
		track_boundary(input_image, output_image, p, p, 5);
	}
	cv::namedWindow("Input Image");
	cv::namedWindow("Output Image");
	cv::imshow("Input Image", input_image);
	cv::imshow("Output Image", output_image);



	cv::waitKey();
	cv::imwrite("output.png", output_image);
	return 0;
}

unsigned char get_density(cv::Mat& image, int x, int y)
{
	if ((x < 0) || (x > image.cols - 1) || (y < 0) || (y > image.rows -
		1)) {
		return 0;
	}
	else {
		return (image.ptr<unsigned char>(y)[x] == 255) ? 1 : 0;
	}
}

void get_neighbors(cv::Mat& image, cv::Point p, unsigned char neighbors[])
{
	neighbors[0] = 0;
	neighbors[1] = get_density(image, p.x + 1, p.y);
	neighbors[2] = get_density(image, p.x + 1, p.y - 1);
	neighbors[3] = get_density(image, p.x, p.y - 1);
	neighbors[4] = get_density(image, p.x - 1, p.y - 1);
	neighbors[5] = get_density(image, p.x - 1, p.y);
	neighbors[6] = get_density(image, p.x - 1, p.y + 1);
	neighbors[7] = get_density(image, p.x, p.y + 1);
	neighbors[8] = get_density(image, p.x + 1, p.y + 1);
}



cv::Point get_tracking_point(cv::Point p, int d)
{
	switch (d) {
	case 1:
		return cv::Point(++p.x, p.y);
	case 2:
		return cv::Point(++p.x, --p.y);
	case 3:
		return cv::Point(p.x, --p.y);
	case 4:
		return cv::Point(--p.x, --p.y);
	case 5:
		return cv::Point(--p.x, p.y);
	case 6:
		return cv::Point(--p.x, ++p.y);
	case 7:
		return cv::Point(p.x, ++p.y);
	case 8:
		return cv::Point(++p.x, ++p.y);
	default:
		return cv::Point(-1, -1);
	}
}
cv::Point find_start_point(cv::Mat& input_image, cv::Mat& output_image, cv
	::Point p)
{
	int x, y;
	bool inside = false;



	y = p.y;
	for (x = p.x; x < input_image.cols; x++) {
		if (input_image.ptr<unsigned char>(y)[x] == 0) {
			inside = false;
			continue;
		}
		if (output_image.ptr<unsigned char>(y)[x] == 255) {
			inside = true;
			continue;
		}
		if (inside == false) {
			return cv::Point(x, y);
		}
		else {
		}
	}
	return cv::Point(-1, -1);
}

void track_boundary(cv::Mat& input_image, cv::Mat& output_image, cv::Point
	p0, cv::Point p, int d)
{
	int i, j;



	unsigned char neighbors[9];
	cv::Point new_p;
	int new_d = 0;
	get_neighbors(input_image, p, neighbors);
	i = d;
	do {
		j = (i % 8) + 1;
		if (neighbors[i] == 0 && neighbors[j] == 1) {
			new_p = get_tracking_point(p, j);
			if (output_image.ptr<unsigned char>(new_p.y)[new_p.x] == 0) {
				new_d = j;
				break;
			}
		}
		i = j;
	} while (i != d);
	if (new_d == 0) {
		return;
	}
	new_p = get_tracking_point(p, new_d);
	if (new_p.x == p0.x && new_p.y == p0.y) {
		return;
	}
	output_image.ptr<unsigned char>(new_p.y)[new_p.x] = 255;


	new_d = (d + 3) % 8 + 1;
	track_boundary(input_image, output_image, p0, new_p, new_d);
}