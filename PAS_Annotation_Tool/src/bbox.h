#ifndef BBOX_H
#define BBOX_H

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

typedef struct BBox_t
{
    BBox_t() : x1(-1), y1(-1), x2(-1), y2(-1)
    {

    }

    BBox_t(int x11, int y11, int x22, int y22)
        : x1(x11), y1(y11), x2(x22), y2(y22)
    {

    }

    int x1, y1, x2, y2;

    int w() { return x2-x1;}

    int h() { return y2-y1;}

}BBox_t;

typedef struct ImageSize_t
{
    int w, h, c;
}ImageSize_t;

/**
 * @brief The BBox class
 */
class BBox
{
public:
    BBox(std::string path,std::string filename);

    bool get_bboxes(std::vector<BBox_t> &bbox);

    bool set_bboxes(const std::vector<BBox_t> &bbox, \
                    const ImageSize_t &img_size);

    bool startswith(std::string s, std::string pattern);

private:
    std::string _bbox_path;

    std::string _bbox_filename;

    ImageSize_t _img_size;

    int _objects;
};

#endif // BBOX_H
