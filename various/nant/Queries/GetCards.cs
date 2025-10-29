using System;
using System.Collections.Generic;
using System.Linq;
using Core;
using Core.DomainModel;

namespace Nant.Queries
{
	public class GetCards
	{
		private readonly TrelloService _service;

		public GetCards(TrelloService service)
		{
			_service = service;
		}

		public List<Card> Execute(Board board, string listName)
		{
			var lists = _service.GetListsForBoard(board);
			if (!lists.Any())
			{
				throw new ArgumentException("'" + board.Name + "' - no lists on board.");
			}

			var list = GetList(lists, listName);
			return _service.GetCardsForList(list).OrderBy(x => x.IdShort).ToList();
		}

		private List GetList(List<List> lists, string listName)
		{
			var list = lists.Where(x => x.Name.ToLower() == listName.ToLower()).ToList();

			if (!list.Any())
			{
				throw new ArgumentException("'" + listName + "' - no such list.");
			}

			if (list.Count() != 1)
			{
				throw new ArgumentException("Two or more lists with name '" + listName + "' exists.");
			}

			return list[0];
		}
	}
}
