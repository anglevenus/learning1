#pragma once
#include <string>
#include <map>
#include <memory>

namespace bigfile
{
	using std::string;

	enum GenerateItemState
	{
		NORMAL = 0,
		CONTENT_CHANGE = 1,
		REFERENCE_CHANGE = 1<<1,

		FILE_NOT_EXIST = 1<<2 | CONTENT_CHANGE,
	};


	class PreGenerateItem
	{
	public:
		PreGenerateItem(string filename);
		~PreGenerateItem();

		void UpdateState();

	private:
		string mFilename;
		//std::string mFilenameMD5;
		string mContentMD5;
		string mReferenceFilename;

		long mVersion = 0L;			//used the change time at the first method
		long mTargetVersion = 0L;
		bool mInCommonBigFile = false;


		GenerateItemState state = NORMAL;
	};


	class PreGenerateManager
	{
	public:
		static PreGenerateManager* GetInstance();
		static void CreateInstance();
		static void DestroyInstance();

		void LoadConfig(const string &filepath);

		void SaveConfig(const string &filepath);

		//update the file
		void CheckAndUpdateFileInfo(const string &filepath);

		void DoPreGenerate();

	private:
		PreGenerateManager();
		~PreGenerateManager();


		std::map<string, std::unique_ptr<PreGenerateItem>> mItems;

		std::map<string, string> mReferenceMap;

		string mPreOutputPath;
		
		static PreGenerateManager *sInstance;
	};

};


