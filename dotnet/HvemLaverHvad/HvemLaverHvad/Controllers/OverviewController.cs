using HvemLaverHvad.Models;
using HvemLaverHvad.Queries;
using HvemLaverHvad.TargetProcess;
using System.Collections.Generic;
using System.Linq;
using System.Web.Mvc;

namespace HvemLaverHvad.Controllers
{
    public class OverviewController : Controller
    {
        const string _storyLinkBase = "https://{your-company}.tpondemand.com/entity/{0}";
        readonly Dictionary<string, OverviewUserModel> _users = new Dictionary<string, OverviewUserModel>();
        readonly Dictionary<string, string> _nameToUserMap = new Dictionary<string, string>();
        readonly RestGet _restGet = new RestGet();

        public ActionResult Index()
        {
            BuildMaps();
            var currentSprint = GetCurrentSprint();
            if (currentSprint == null)
            {
                return View("NoCurrentSprint");
            }
            Session["Sprint"] = currentSprint.Name;

            var userStories = GetUserStories(currentSprint.Id);
            var model = GetModel(userStories);
            return View(model);
        }

        void BuildMaps()
        {
            MapsAdd("user1@your-company", "FirstName LastName");
            MapsAdd("user2@your-company", "FirstName LastName", new List<string> { "Alias1", "Alias2" });
        }

        void MapsAdd(string login, string name, List<string> aliases = null)
        {
            _users.Add(login, new OverviewUserModel { Name = name });
            _nameToUserMap.Add(name.ToLower(), login);
            if (aliases == null || !aliases.Any())
            {
                return;
            }

            foreach (var alias in aliases)
            {
                _nameToUserMap.Add(alias.ToLower(), login);
            }
        }

        TeamIteration GetCurrentSprint()
        {
            var teamIterations = _restGet.Execute<TeamIterations>("TeamIterations");
            return teamIterations.Items.SingleOrDefault(x => x.IsCurrent);
        }

        List<UserStory> GetUserStories(int sprintId)
        {
            var allUserStories = new List<UserStory>();
            var basePathFormatter = "{0}?include=[Name,AssignedUser,TeamIteration]&format=json&where=EntityState.Name%20eq%20'In%20Progress'";

            foreach (var methodPair in new string[] { "Bugs/Bug", "Tasks/Task", "UserStories/Story", "Features/Feature"})
            {
                var pairs = methodPair.Split(new[] { '/' });
                var method = pairs[0];
                var storyType = pairs[1];
                var url = string.Format(basePathFormatter, method);

                while (!string.IsNullOrWhiteSpace(url))
                {
                    var userStories = _restGet.Execute<UserStories>(url);
                    if (userStories == null || userStories.Items == null)
                    {
                        break;
                    }

                    var stories = userStories.Items.Where(x => x.TeamIteration != null && x.TeamIteration.Id == sprintId).ToList();
                    if (stories.Any())
                    {
                        stories.ForEach(x => x.StoryType = storyType);
                        allUserStories.AddRange(stories);
                    }

                    url = userStories.Next;
                }
            }

            return allUserStories;
        }

        OverviewModel GetModel(List<UserStory> userStories)
        {
            var model = new OverviewModel();
            foreach (var story in userStories)
            {
                foreach (var user in story.AssignedUser.Items)
                {
                    var login = TranslateLogin(user.Login);
                    if (!_users.ContainsKey(login))
                    {
                        continue;
                    }

                    var task = new OverviewUserTaskModel
                    {
                        Id = story.Id,
                        Link = string.Format(_storyLinkBase, story.Id),
                        Name = story.Name,
                        StoryType = story.StoryType,
                    };
                    _users[login].Tasks.Add(task);
                }
            }

            foreach (var entry in _users)
            {
                var user = entry.Value;
                SetupUser(user);
                model.Users.Add(user);
            }

            model.Users.Sort();
            return model;
        }

        void SetupUser(OverviewUserModel user)
        {
            user.Tasks.Sort();

            var taskCount = user.Tasks.Count;
            if (taskCount == 0)
            {
                user.Status = " slapper af!";
                user.Emoji = "relax.png";
                user.BackgroundColor = "white";
            }
            else if (taskCount == 1)
            {
                user.Status = " fokuserer på:";
                user.Emoji = "focused.png";
                user.BackgroundColor = "white";
            }
            else if (taskCount == 2)
            {
                user.Status = " arbejder med:";
                user.Emoji = "busy.png";
                user.BackgroundColor = "white";
            }
            else if (taskCount == 3)
            {
                user.Status = " sveder over:";
                user.Emoji = "much.png";
                user.BackgroundColor = "#ffffa0";
            }
            else
            {
                user.Status = " stresser over:";
                user.Emoji = "toomuch.png";
                user.BackgroundColor = "#ffa0a0";
            }
        }

        string TranslateLogin(string login)
        {
            if (login.IndexOf('@') >= 0)
            {
                return login;
            }

            var key = login.ToLower();
            if (_nameToUserMap.ContainsKey(key))
            {
                return _nameToUserMap[key];
            }

            return login;
        }
    }
}
