using System;
using System.Linq;
using Core;
using Core.DomainModel;

namespace Nant.Queries
{
	public class GetBoardByName
	{
		private readonly TrelloService _service;

		public GetBoardByName(TrelloService service)
		{
			_service = service;
		}

		public Board Execute(string username, string boardName)
		{
			var boards = _service.GetBoards(username);
			if (!boards.Any())
			{
				throw new ArgumentException("No boards exists for '" + username + "' on Trello.");
			}

			var board = boards.Where(x => String.Equals(x.Name, boardName, StringComparison.CurrentCultureIgnoreCase)).ToList();
			var count = board.Count();
			if (count == 0)
			{
				throw new ArgumentException("The specified board '" + boardName + "' does not exist for '" + username + "' on Trello.");
			}
			if (count > 1)
			{
				throw new ArgumentException("Multiple boards named '" + boardName + "' exists for '" + username + "' on Trello.");
			}

			return board[0];
		}
	}
}
