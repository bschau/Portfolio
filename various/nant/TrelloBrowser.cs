using System;
using System.Collections.Generic;
using System.Linq;
using Core;
using Core.DomainModel;
using Nant.Builders;
using Nant.DomainModel;
using Nant.Queries;
using NAnt.Core;
using NAnt.Core.Attributes;

namespace Nant
{
	[TaskName("trellobrowser")]
	class TrelloBrowser : Task
	{
		private const string _property = "trellobrowser.selection";

		[TaskAttribute("board", Required = true)]
		[StringValidator(AllowEmpty = false)]
		public string Board { get; set; }

		[TaskAttribute("property", Required = false)]
		[StringValidator(AllowEmpty = true)]
		public string Property { get; set; }

		protected override void ExecuteTask()
		{
			try
			{
				var settings = new SettingsBuilder().Build();
				var service = new TrelloServiceBuilder(settings).Build();
				var board = new GetBoardByName(service).Execute(settings.Username, Board);
				var overview = GetOverview(service, board);

				RunLoop(service, overview);
			}
			catch (Exception exception)
			{
				throw new BuildException(exception.Message, Location);
			}
		}

		private List<OverviewListItem> GetOverview(TrelloService service, Board board)
		{
			var lists = service.GetListsForBoard(board).OrderBy(x => x.Name);
			var overview = new List<OverviewListItem>();
			var index = 1;
			foreach (var overviewListItem in lists.Select(list => new OverviewListItem
			{
				Id = index.ToString(),
				TrelloList = list,
			}))
			{
				index++;
				overview.Add(overviewListItem);
			}

			return overview;
		}

		private void RunLoop(TrelloService service, List<OverviewListItem> overview)
		{
			while (true)
			{
				Console.WriteLine("Lists on board '{0}'", Board);
				foreach (var overviewListItem in overview)
				{
					Console.WriteLine("{0}.\t{1}", overviewListItem.Id, overviewListItem.TrelloList.Name);
				}
				Console.WriteLine("Please enter number or type   e{number}  to examine board...");
				var response = Console.ReadLine();
				if (string.IsNullOrWhiteSpace(response))
				{
					continue;
				}
				if (response[0] == 'e')
				{
					var list = GetListById(overview, response.Substring(1));
					if (list == null)
					{
						continue;
					}

					ExamineList(service, list);
				}
				else
				{
					var list = GetListById(overview, response);
					if (list == null)
					{
						continue;
					}

					if (string.IsNullOrEmpty(Property))
					{
						Properties[_property] = list.Name;
					}
					else
					{
						Properties[Property] = list.Name;
					}
					return;
				}
			}
		}

		private List GetListById(List<OverviewListItem> overview, string id)
		{
			var list = overview.Where(x => x.Id == id.Trim()).ToList();
			if (list.Count() != 1)
			{
				Console.WriteLine("Illegal list.");
				return null;
			}

			return list[0].TrelloList;
		}

		private void ExamineList(TrelloService service, List list)
		{
			var cards = service.GetCardsForList(list).OrderBy(x => x.IdShort);

			if (!cards.Any())
			{
				Console.WriteLine("No cards on list '{0}'", list.Name);
			}

			foreach (var card in cards)
			{
				Console.WriteLine("{0}:\t{1}", card.IdShort, card.Name);
			}
			Console.WriteLine("Enter to return.");
			Console.ReadLine();
		}
	}
}
