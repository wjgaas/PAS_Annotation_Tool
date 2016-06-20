/**
 * @Author:      JianGuo Wang
 * 
 * @DateTime:    2016-03-31 15:22:37
 * 
 * @Description: this is a class to operate the folders and files
 * 
 */

#include "filesupport.h"

/**
* @brief      { list sub level of folder under the path }
*
* @param[in]  path  { folder path }
*
* @return     { vector of subfolders }
*/
FolderList FileSupport::list_folders(String path)
{
	FolderList folders;
	long handle;
	_finddata_t fd;

	get_handle(fd, handle, path);

	do
	{
		if(is_dir(fd) && !is_current(fd) && !is_parent(fd))
			folders.push_back(path + "\\" + fd.name);
	}
	while(_findnext(handle, &fd) == 0);

	_findclose(handle);

	return folders;
}

/**
* @brief      { list files under the path }
*
* @param[in]  path  { folder path }
*
* @return     { vector of subfiles }
*/
FileList FileSupport::list_files(String path, bool base)
{
	FileList files;
	long handle;
	_finddata_t fd;

	get_handle(fd, handle, path);

	do
	{
		if(!is_dir(fd))
        {
            if(base)
                files.push_back(path + "\\" + fd.name);
            else
                files.push_back(fd.name);
        }
	}
	while(_findnext(handle, &fd) == 0);

	_findclose(handle);

	return files;
}

/**
* @brief      { list all files under the path recurrently }
*
* @param[in]  path   { folder path }
* @param      files  { vector of all subfiles }
*/
void FileSupport::list_files_recurrently(String path, FileList &files)
{
	long handle;
	_finddata_t fd;

	get_handle(fd, handle, path);

	do
	{
		if(is_dir(fd))
		{
			if(!is_current(fd) && !is_parent(fd))
				list_files_recurrently(path + "\\" + fd.name, files);
		}
		else
		{
			files.push_back(fd.name);
		}
	}
	while( _findnext(handle, &fd) == 0 );

	_findclose(handle);
}

/**
* @brief      { count the lines of a file }
*
* @param[in]  filename  { path of file }
*
* @return     { how many lines }
*/
int FileSupport::get_file_line_num(String filename)
{
	int lines=0;

	std::ifstream r_file(filename.c_str(), std::ios::in);
	String temp;

	if(!r_file.good())
		return -1;
	else
	{
		while(getline(r_file, temp))
			lines++;

		r_file.close();
	}

	return lines;
}

bool FileSupport::check_file_exist(String filename)
{
    if(_access(filename.c_str(),0) == -1)
    {
        std::cerr << "file do not exist : " \
                  << filename << std::endl;
        return false;
    }
    return true;
}


/**
* @brief      { get the handle and _finddata_t of a path }
*
* @param      fd      { _finddata_t }
* @param      handle  { handle }
* @param      path    { path of a folder }
*/
void FileSupport::get_handle(_finddata_t &fd, long &handle, String &path)
{
	while(path[path.size()-1]=='\\') path.erase(path.size()-1);

	handle = _findfirst((path + "\\*").c_str(), &fd);

	check_handle(handle);
}

/**
* @brief      { check file handle is valid }
*
* @param[in]  handle  { file handle }
*/
void FileSupport::check_handle(long handle)
{
	if(handle == -1L)
	{
		std::cerr << "no such path" << std::endl;
        system("pause");
        //exit(-1);
	}
}

/**
* @brief      { check is it a director }
*
* @param[in]  fd    { _finddata_t }
*
* @return     { true if is a dir; false if not a dir }
*/
bool FileSupport::is_dir(_finddata_t fd)
{
    return (fd.attrib & _A_SUBDIR);
}

/**
* @brief      { check if is current dir }
*
* @param[in]  fd    { _finddata_t }
*
* @return     { true if is current dir; false if not }
*/
bool FileSupport::is_current(_finddata_t fd)
{
	return (strcmp(fd.name, ".") == 0);
}

/**
* @brief      { check if is parent dir }
*
* @param[in]  fd    { _finddata_t }
*
* @return     { true if is parent dir; false if not }
*/
bool FileSupport::is_parent(_finddata_t fd)
{
	return (strcmp(fd.name, "..") == 0);
}
