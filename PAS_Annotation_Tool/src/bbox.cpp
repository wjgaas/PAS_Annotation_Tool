#include "bbox.h"
#include "filesupport.h"

/**
 * @brief BBox::BBox
 * @param path
 * @param filename
 */
BBox::BBox(std::string path, std::string filename)
    : _bbox_path(path), _bbox_filename(filename)
{
}

/**
 * @brief BBox::get_bboxes
 * @param bbox
 * @return
 */
bool BBox::get_bboxes(std::vector<BBox_t> &bbox)
{
    std::string bbox_filename(_bbox_path + "\\" + _bbox_filename);

    FileSupport fs;

    if(!fs.check_file_exist(bbox_filename)) return false;

    if(fs.get_file_line_num(bbox_filename) < 11) return false;

    std::ifstream annotationfile(bbox_filename, std::ios::in);

    if(!annotationfile.good())
    {
        std::cerr << "open annotation file error" << std::endl;
        return false;
    }

    std::string temp;

    getline(annotationfile, temp);
    getline(annotationfile, temp);
    getline(annotationfile, temp);
    getline(annotationfile, temp);  //Image size (X x Y x C) :

    if(startswith(temp, "Image size (X x Y x C) :"))
    {
        sscanf(temp.c_str(), "Image size (X x Y x C) : %d x %d x %d", \
               &_img_size.h, &_img_size.w, &_img_size.c);
    }

    getline(annotationfile, temp);  //Database :
    getline(annotationfile, temp);  //Objects with

    if(startswith(temp, "Objects with ground truth :"))
    {
        sscanf(temp.c_str(), "Objects with ground truth : %d { "" "" "" "" "" }", \
               &_objects);
    }

    getline(annotationfile, temp);
    getline(annotationfile, temp);
    getline(annotationfile, temp);
    getline(annotationfile, temp);
    getline(annotationfile, temp);  //Top left pixel

    for(int i=0; i<_objects; i++)
    {
        if(!getline(annotationfile, temp)) return false;
        if(!getline(annotationfile, temp)) return false;
        if(!getline(annotationfile, temp)) return false;
        if(!getline(annotationfile, temp)) return false;

        int objn;
        BBox_t box;

        sscanf(temp.c_str(), "Bounding box for object %d \"\" (Xmin, Ymin) - (Xmax, Ymax) : (%d, %d) - (%d, %d)", \
               &objn, &box.x1, &box.y1, &box.x2, &box.y2);

        bbox.push_back(box);
    }

    annotationfile.close();

    return true;
}

/**
 * @brief BBox::set_bboxes
 * @param bbox
 * @param img_size
 * @return
 */
bool BBox::set_bboxes(const std::vector<BBox_t> &bbox, const ImageSize_t &img_size)
{
    int nobjs = bbox.size();

    FileSupport fs;
    if(!fs.check_file_exist(_bbox_path)) return false;

    std::string bbox_filename(_bbox_path + "\\" + _bbox_filename);

    std::ofstream annotationfile(bbox_filename, std::ios::out);

    if(!annotationfile.good())
    {
        std::cerr << "open annotation file error : " << bbox_filename << std::endl;
        return false;
    }

    annotationfile << "# PASCAL Annotation Version 1.00" << std::endl;
    annotationfile << std::endl;
    annotationfile << "Image filename : \"" << bbox_filename << "\"" << std::endl;
    annotationfile << "Image size (X x Y x C) : " \
                   << img_size.h << " x " << img_size.w << " x " << img_size.c << std::endl;
    annotationfile << "Database : \"UESTC-IMAGE-PROCESSING-LAB\"" << std::endl;
    annotationfile << "Objects with ground truth : " << nobjs \
                   << " { \"\" \"\" \"\" \"\" }" << std::endl;
    annotationfile << std::endl;
    annotationfile << "# Note that there might be other objects in the image" << std::endl;
    annotationfile << "# for which ground truth data has not been provided." << std::endl;
    annotationfile << std::endl;
    annotationfile << "# Top left pixel co-ordinates : (1, 1)" << std::endl;

    for(int i=0; i<nobjs; i++)
    {
        annotationfile << std::endl;
        annotationfile << "# Details for object " << i+1 << " (\"\")" << std::endl;
        annotationfile << "Original label for object " << i+1 << " \"\" : \"\"" << std::endl;
        annotationfile << "Bounding box for object " << i+1 << " \"\" (Xmin, Ymin) - (Xmax, Ymax) :" \
                       << "(" << bbox[i].x1 << ", " << bbox[i].y1 << ") - (" \
                       << bbox[i].x2 << ", " << bbox[i].y2 << ")" << std::endl;
    }

    return true;
}

/**
 * @brief BBox::startswith
 * @param s
 * @param pattern
 * @return
 */
bool BBox::startswith(std::string s, std::string pattern)
{
    if(s.size() < pattern.size()) return false;

    for(size_t i=0; i<pattern.size(); ++i)
    {
        if(s[i] != pattern[i])
            return false;
    }

    return true;
}
