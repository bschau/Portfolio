using System;
using System.IO;
using System.Linq;

namespace kweddl
{
	public class SaveMusic
	{
		readonly char[] _invalidFileNameChars = Path.GetInvalidFileNameChars();
        readonly string _home;

        public SaveMusic(string home)
        {
            _home = home;
        }

		public void Execute(KwedItem kwedItem, byte[] data)
		{
			var filename = new string(kwedItem.Title
                    .Where(c => !_invalidFileNameChars.Contains(c)).ToArray());
			var downloadTo = Path.Combine(_home, "Desktop");
			filename = Path.Combine(downloadTo, $"{filename}.mp3");
			File.WriteAllBytes(filename, data);
        }
    }
}
