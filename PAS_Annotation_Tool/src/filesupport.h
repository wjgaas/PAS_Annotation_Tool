/**
 * @Author:      JianGuo Wang
 * 
 * @DateTime:    2016-03-31 15:22:37
 * 
 * @Description: this is a class to operate the folders and files
 * 
 */

#ifndef __FILE_SUPPORT_H__
#define __FILE_SUPPORT_H__

#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <io.h>

namespace WL
{
	class FileSupport;
}

typedef std::string String;

typedef std::vector<String> FileList;

typedef std::vector<String> FolderList;

 /**
  * @brief      { class for file process }
  */
class FileSupport
{
public:

    /**
     * @brief      { list sub level of folder under the path }
     *
     * @param[in]  path  { folder path }
     *
     * @return     { vector of subfolders }
     */
    FolderList list_folders(String path);

    /**
     * @brief      { list files under the path }
     *
     * @param[in]  path  { folder path }
     *
     * @return     { vector of subfiles }
     */
    FileList list_files(String path, bool base=true);

    /**
     * @brief      { list all files under the path recurrently }
     *
     * @param[in]  path   { folder path }
     * @param      files  { vector of all subfiles }
     */
    void list_files_recurrently(String path, FileList &files);

    /**
     * @brief      { count the lines of a file }
     *
     * @param[in]  filename  { path of file }
     *
     * @return     { how many lines }
     */
    int get_file_line_num(String filename);

    bool check_file_exist(String filename);

private:

    /**
     * @brief      { get the handle and _finddata_t of a path }
     *
     * @param      fd      { _finddata_t }
     * @param      handle  { handle }
     * @param      path    { path of a folder }
     */
    void get_handle(_finddata_t &fd, long &handle, String &path);

    /**
     * @brief      { check file handle is valid }
     *
     * @param[in]  handle  { file handle }
     */
    void check_handle(long handle);

    /**
     * @brief      { check is it a director }
     *
     * @param[in]  fd    { _finddata_t }
     *
     * @return     { true if is a dir; false if not a dir }
     */
    bool is_dir(_finddata_t fd);

    /**
     * @brief      { check if is current dir }
     *
     * @param[in]  fd    { _finddata_t }
     *
     * @return     { true if is current dir; false if not }
     */
    bool is_current(_finddata_t fd);

    /**
     * @brief      { check if is parent dir }
     *
     * @param[in]  fd    { _finddata_t }
     *
     * @return     { true if is parent dir; false if not }
     */
    bool is_parent(_finddata_t fd);

};

#endif // __FILE_SUPPORT_H__
