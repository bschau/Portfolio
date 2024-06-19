namespace HvemLaverHvad.TargetProcess
{
    public class UserStory
    {
        public string Id { get; set; }
        public string Name { get; set; }
        public AssignedUsers AssignedUser { get; set; }
        public TeamIteration TeamIteration { get; set; }
        public string StoryType { get; set; }
    }
}
