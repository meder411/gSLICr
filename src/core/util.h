#ifndef SUPERPIXELS_SRC_CORE_UTIL_H_
#define SUPERPIXELS_SRC_CORE_UTIL_H_

#include <boost/filesystem.hpp>
#include <boost/range/iterator_range.hpp>

#include <string>
#include <exception>
#include <iostream>

#define EXCEPTION_THROWER(ET, message)									\
	{																	\
		ET e;															\
		e.setMessage(message);											\
		e.setLocation(__func__, __FILE__, __LINE__);					\
		throw e;														\
	}

namespace Util
{
	namespace Exception
	{
		// General exception class
		struct UtilException : public std::exception
		{
			std::string _msg;

			UtilException(const std::string &exception_name) : _msg(exception_name) {}

			void setMessage(const std::string &message)
			{
				_msg += ": " + message + "\n";
			}

			void setLocation(const char * func, const char * file, const int line)
			{
				_msg += "  In function " + std::string(func) + "(" + file + ":" + std::to_string(line) + ")";
			}

			const char * what() const throw()
			{
				return _msg.c_str();
			}
		};

		// Specializations of the UtilException
		struct FileNotFoundException : public UtilException
		{
			FileNotFoundException() : UtilException("FileNotFoundException") {}
		};
		struct IOException : public UtilException
		{
			IOException() : UtilException("IOException") {}
		};

	} // namespace Util::Exception

	namespace Files
	{
		inline const std::string joinPathAndFile(const std::string &path, const std::string &fname)
		{
			const boost::filesystem::path b_fname(fname);
			const boost::filesystem::path b_path(path);
			const boost::filesystem::path b_full_path = b_path / b_fname;
			return b_full_path.string();
		}

		inline const std::string getFilenameFromPath(const std::string &path)
		{
			const boost::filesystem::path b_path(path);
			return b_path.filename().string();
		}

		inline const std::string getFilenameNoExtensionFromPath(const std::string &path)
		{
			const boost::filesystem::path b_path(path);
			return b_path.stem().string();

		}
		
		inline const std::string getBasePathFromPath(const std::string &path)
		{
			boost::filesystem::path b_path(path);
			return b_path.remove_filename().string();
		}

		inline const std::string getFileExtension(const std::string &file)
		{
			return boost::filesystem::extension(file);
		}

		inline const std::string stripRootDir(const std::string &path)
		{
			boost::filesystem::path b_path(path);
			const boost::filesystem::path &parent_path = b_path.parent_path();
			if (parent_path.empty() || parent_path.string() == "/")
			{
				return boost::filesystem::path().string();
			}
			else
			{
				return (stripRootDir(parent_path.string()) / b_path.filename()).string();
			}
		}

		/**
		 * Create a directory and all necessary parents
		 *
		 * @param path 						Path to the new directory
		 *
		 * @return True if call is successful, False otherwise
		 */
		inline const bool mkdirs(const std::string &path)
		{
			const boost::filesystem::path b_path(path);
			if (boost::filesystem::is_directory(b_path)) { return false; }
			return boost::filesystem::create_directories(b_path);
		}

		/**
		 * Create a directory
		 *
		 * @param path 						Path to the new directory
		 *
		 * @return True if call is successful, False otherwise
		 */
		inline const bool mkdir(const std::string &path)
		{
			const boost::filesystem::path b_path(path);
			if (boost::filesystem::is_directory(b_path)) { return false; }
			return boost::filesystem::create_directory(b_path);
		}

		/**
		 * Check if a path is to a directory
		 *
		 * @param path 						Path to check
		 *
		 * @return True if it is a directory, False otherwise
		 */
		inline const bool isDir(const std::string &path)
		{
			const boost::filesystem::path b_path(path);
			return boost::filesystem::is_directory(b_path);
		}

		inline const bool exists(const std::string &path)
		{
			const boost::filesystem::path b_path(path);
			return boost::filesystem::exists(b_path);
		}

		/**
		 * Get absolute paths to all files in a directory
		 *
		 * @param files						Vector to hold filenames (Note: contains relative paths from root)
		 * @param root						Path to directory to search
		 * @param ext						File type extension (with leading period)
		 * @param recursive					Whether or not to recurse through subdirectories
		 *
		 * @return None
		 */
		inline void getFilesOfTypeInDirectory(std::vector<std::string> &files,
			const std::string &root, const std::string &ext, const bool recursive)
		{
			const boost::filesystem::path path = boost::filesystem::path(root);
			if (!boost::filesystem::exists(path))
			{
				EXCEPTION_THROWER(Util::Exception::IOException, "Error: Path does not exist ('" + root + "')");
			}
			if (!boost::filesystem::is_directory(path))
			{
				EXCEPTION_THROWER(Util::Exception::IOException, "Error: Path is not a directory ('" + root + "')");
			}

			// Recursively traverse filetree
			if (recursive)
			{
				for (auto &file : boost::make_iterator_range(boost::filesystem::recursive_directory_iterator(path), {}))
				{
					if (file.path().extension().string() == ext)
					{
						files.push_back(file.path().string());
					}
				}
			}
			else
			{
				for (auto &file : boost::make_iterator_range(boost::filesystem::directory_iterator(path), {}))
				{
					if (file.path().extension().string() == ext)
					{
						files.push_back(file.path().string());
					}
				}	
			}
			std::sort(files.begin(), files.end());
		}

	} // namespace Util::Files
	
} // namespace Util

#endif // SUPERPIXELS_SRC_CORE_UTIL_H_