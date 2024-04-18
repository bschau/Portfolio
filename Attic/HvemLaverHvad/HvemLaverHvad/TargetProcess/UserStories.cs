using System.Collections.Generic;

namespace HvemLaverHvad.TargetProcess
{
    public class UserStories
    {
        public UserStories()
        {
            Items = new List<UserStory>();
        }

        public string Next { get; set; }
        public List<UserStory> Items { get; set; }
    }
}
