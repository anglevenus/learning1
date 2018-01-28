#include "PreGenerate.h"
#include <assert.h>
#include "tinyxml2.h"

#include "md5.h"

namespace bigfile {

	string MakeMD5(const string &src) 
	{
		MD5 md5;
		md5.update(src.c_str(), src.length());
		return md5.toString();
	}



	PreGenerateItem::PreGenerateItem(string filename)
		:mFilename(filename)
	{

	}

	PreGenerateItem::~PreGenerateItem()
	{
	}

	void PreGenerateItem::UpdateState()
	{
		int result = 0;
		struct stat buf;

		result = stat(mFilename.c_str(), &buf);

		if (result != 0)
		{
			this->state = FILE_NOT_EXIST;
			mTargetVersion = 0;
		}
		else
		{
			long time = buf.st_mtime;

			if (mVersion != time )
			{
				this->state = CONTENT_CHANGE;
				mTargetVersion = time;
			}
		}
	}



	PreGenerateManager* PreGenerateManager::sInstance = nullptr;

	PreGenerateManager * bigfile::PreGenerateManager::GetInstance()
	{
		assert(sInstance != nullptr);
		return sInstance;
	}

	void PreGenerateManager::CreateInstance()
	{
		if (sInstance == nullptr)
			sInstance = new PreGenerateManager();
	}

	void PreGenerateManager::DestroyInstance()
	{
		delete sInstance;
		sInstance = nullptr;
	}

	void PreGenerateManager::LoadConfig(const string &filepath)
	{
		using namespace tinyxml2;

		XMLDocument doc;
		XMLError error = doc.LoadFile(filepath.c_str());
		if (error == XMLError::XML_ERROR_FILE_NOT_FOUND)
			return;

		//TODO:


	}

	void PreGenerateManager::SaveConfig(const string &filepath)
	{
		//TODO:
	}


	void PreGenerateManager::CheckAndUpdateFileInfo(const string & filepath)
	{
		string filenameMd5 = MakeMD5(filepath);

		if (mItems.find(filenameMd5) == mItems.end())
		{
			//create new preitem
			mItems[filepath] = std::unique_ptr<PreGenerateItem> (new PreGenerateItem(filepath));
		}

		std::unique_ptr<PreGenerateItem> &p = mItems[filepath];

		//update the state
		p.get()->UpdateState();
	}

	void PreGenerateManager::DoPreGenerate() 
	{

	}


	PreGenerateManager::PreGenerateManager()
	{
	}

	PreGenerateManager::~PreGenerateManager()
	{
		mItems.clear();
	}



};

