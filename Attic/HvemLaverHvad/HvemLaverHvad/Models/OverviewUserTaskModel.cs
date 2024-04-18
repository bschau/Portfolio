using System;

namespace HvemLaverHvad.Models
{
    public class OverviewUserTaskModel : IComparable
    {
        public string Id { get; set; }
        public string Link { get; set; }
        public string Name { get; set; }
        public string StoryType { get; set; }

        public int CompareTo(object obj)
        {
            return string.Compare(Name, ((OverviewUserTaskModel)obj).Name, StringComparison.CurrentCulture);
        }
    }
}
