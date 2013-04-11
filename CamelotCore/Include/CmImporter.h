#pragma once

#include "CmPrerequisites.h"
#include "CmModule.h"

namespace CamelotEngine
{
	/**
	 * @brief	Module responsible for importing various asset types and converting
	 * 			them to types usable by the engine.
	 */
	class CM_EXPORT Importer : public Module<Importer>
	{
	public:
		/**
		 * @brief	Constructor.
		 */
		Importer(); 
		~Importer(); 

		/**
		 * @brief	Imports a resource at the specified location, and returns the loaded data.
		 *
		 * @param	inputFilePath	Pathname of the input file.
		 * @param	importOptions	(optional) Options for controlling the import.
		 */
		HResource import(const String& inputFilePath, ConstImportOptionsPtr importOptions = nullptr);

		/**
		 * @brief	Automatically detects the importer needed for the provided file and returns valid type of
		 * 			import options for that importer.
		 *
		 * @param	inputFilePath	Pathname of the input file.
		 *
		 * @return	The new import options.
		 * 			
		 * @note	You will need to type cast the importer options to a valid type,
		 * 			taking into consideration exact importer you expect to be used for this file type.
		 * 			If you don't use a proper import options type, an exception will be thrown during import.
		 * 			
		 *			nullptr is returned if the file path is not valid, or if a valid importer cannot be found for
		 *			the specified file.
		 */
		ImportOptionsPtr createImportOptions(const String& inputFilePath);

		/**
		 * @brief	Checks if we can import a file with the specified extension.
		 *
		 * @param	extension	The extension without leading dot.
		 */
		bool supportsFileType(const String& extension) const;

		/**
		 * @brief	Checks if we can import a file with the specified magic number.
		 *
		 * @param	magicNumber 	The buffer containing the magic number.
		 * @param	magicNumSize	Size of the magic number buffer.
		 */
		bool supportsFileType(const UINT8* magicNumber, UINT32 magicNumSize) const;

		/**
		 * @brief	Adds a new asset importer for the specified file extension. If an asset importer for that extension
		 * 			already exists, it is removed and replaced with the current one.
		 * 			
		 * @note	This method should only be called by asset importers themselves on startup.
		 *
		 * @param [in]	importer	The importer that is able to handle files with the specified extension. nullptr if you
		 * 							want to remove an asset importer for the extension.
		 */
		void registerAssetImporter(SpecificImporter* importer);
	private:
		vector<SpecificImporter*>::type mAssetImporters;

		SpecificImporter* getImporterForFile(const String& inputFilePath) const;
	};
}