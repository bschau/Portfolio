using System;
using System.IO;
using System.Linq;

namespace kweddl
{
	public class SaveMusic
	{
		readonly char[] _invalidFileNameChars = Path.GetInvalidFileNameChars();
        readonly string _desktopFolder;

        public SaveMusic(string desktopFolder)
        {
			_desktopFolder = desktopFolder;
        }

		public void Execute(KwedItem kwedItem, byte[] data)
		{
			var filename = new string(kwedItem.Title
                    .Where(c => !_invalidFileNameChars.Contains(c)).ToArray());
			filename = Path.Combine(_desktopFolder, $"{filename}.mp3");
			File.WriteAllBytes(filename, data);
        }
    }
}
