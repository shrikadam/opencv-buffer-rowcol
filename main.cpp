#include <iostream>
#include <vector>
#include <chrono>
#include <opencv2/opencv.hpp>

int main()
{
    cv::Mat im = cv::imread("../test.png");
    int width = im.cols;
    int height = im.rows;
    std::vector<uchar *> lineBuffer;

    ///////////////////// Column-Wise Scanning //////////////////////////

    auto start_c = std::chrono::high_resolution_clock::now();

    size_t line_size = height * 3;
    for (int i = 0; i < width; i++)
    {
        cv::Rect roiv(cv::Point(i, 0), cv::Size(1, height));
        cv::Mat line = im(roiv).clone();
        const uchar *data = (const uchar *)line.data;
        uchar *lineBuf = new uchar[line_size];
        memcpy(lineBuf, data, line_size);
        // cv::Mat lineimg = cv::Mat(height, 1, CV_8UC3, lineBuf);
        // cv::imshow("test", lineimg);
        // cv::waitKey(0);
        lineBuffer.push_back(lineBuf);
    }

    cv::Mat stitchedImage = cv::Mat(cv::Size(lineBuffer.size(), height), CV_8UC3);
    cv::Mat lineImage = cv::Mat(cv::Size(1, height), CV_8UC3);

    // for (int col = 0; col < stitchedImage.cols; col++)
    // {
    //     uchar *buffdata = lineBuffer[col];
    //     int buff_index = 0;
    //     for (int row = 0; row < stitchedImage.rows; row++)
    //     {
    //         // stitchedImage.at<cv::Vec3b>(col, row)[0] = buffdata[buff_index++];
    //         // stitchedImage.at<cv::Vec3b>(col, row)[1] = buffdata[buff_index++];
    //         // stitchedImage.at<cv::Vec3b>(col, row)[2] = buffdata[buff_index++];
    //         int pixel_index = row * stitchedImage.cols + col;
    //         uchar *data = stitchedImage.ptr<uchar>(pixel_index);
    //         memcpy(data, buffdata, 3);
    //     }
    // }

    for (size_t i = 0; i < lineBuffer.size(); ++i)
    {
        cv::Rect roi_target(cv::Point(i, 0), cv::Size(1, height));
        uchar *data = lineImage.ptr<uchar>(0);
        memcpy(data, lineBuffer[i], line_size);
        lineImage.copyTo(stitchedImage(roi_target));
        // cv::imshow("test", lineImage);
        // cv::waitKey(0);
    }

    cv::imwrite("../result_column_wise.png", stitchedImage);

    auto end_c = std::chrono::high_resolution_clock::now();

    for (auto i : lineBuffer)
        delete[] i;
    lineBuffer.clear();
    //////////////////////// Row-Wise Scanning /////////////////////////

    auto start_r = std::chrono::high_resolution_clock::now();

    line_size = width * 3; // 3 Channels B, G, R

    for (int i = 0; i < height; i++)
    {
        cv::Rect roih(cv::Point(0, i), cv::Size(width, 1));
        cv::Mat line = im(roih);
        const uchar *data = (const uchar *)line.data;
        uchar *lineBuf = new uchar[line_size];
        memcpy(lineBuf, data, line_size);
        lineBuffer.push_back(lineBuf);
    }

    stitchedImage = cv::Mat(cv::Size(width, lineBuffer.size()), CV_8UC3);

    for (size_t i = 0; i < lineBuffer.size(); ++i)
    {
        uchar *data = stitchedImage.ptr<uchar>(i);
        memcpy(data, lineBuffer[i], line_size);
    }

    cv::imwrite("../result_row_wise.png", stitchedImage);

    auto end_r = std::chrono::high_resolution_clock::now();

    ////////////////////////////////////////////////////////////////////

    auto dur_c = std::chrono::duration_cast<std::chrono::milliseconds>(end_c - start_c);
    auto dur_r = std::chrono::duration_cast<std::chrono::milliseconds>(end_r - start_r);

    std::cout << "Column wise: " << dur_c.count() << "\n";
    std::cout << "Row wise: " << dur_r.count() << "\n";

    for (auto i : lineBuffer)
        delete[] i;
    lineBuffer.clear();
}