using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using VPSApps.Commands;
using VPSApps.DomainModel;

namespace VPSApps.Modules
{
	internal class Eventor : IModule
	{
		const string _moduleName = "Eventor";
		readonly VpsAppsRc _vpsAppsRc;
		int _thisYear;
		int _thisYday;
		int _days;

		public Eventor(VpsAppsRc vpsAppsRc)
		{
			_vpsAppsRc = vpsAppsRc;
		}

		public void Execute()
		{
			var now = DateTime.Now;
			_thisYear = now.Year;
			_thisYday = now.DayOfYear;
			_days = new DateTime(_thisYear, 12, 31).DayOfYear;

			var eventsFilePath = GetEventsFilePath();
			var events = GetEvents(eventsFilePath);
			if (!events.Any())
			{
				return;
			}

			var htmlBuilder = new HtmlBuilder(_moduleName);
			foreach (var eventorEvent in events)
			{
				htmlBuilder.AppendFormat("<p>{0} {1}</p>", eventorEvent.Glyph, eventorEvent.Text);
			}

			var sendEmail = new SendEmail(_vpsAppsRc.From, _vpsAppsRc.To);
			sendEmail.Execute(htmlBuilder.Title, htmlBuilder.ToString());
		}

		string GetEventsFilePath()
		{
			if (!string.IsNullOrWhiteSpace(_vpsAppsRc.EventsFilePath))
			{
				return _vpsAppsRc.EventsFilePath;
			}

			return Path.Combine(Configuration.Home, Configuration.IsUnix
					? ".eventor"
					: "_eventor");
		}

		List<EventorEvent> GetEvents(string path)
		{
			var events = new List<EventorEvent>();
			var allEvents = File.ReadAllLines(path);
			for (var i = 0; i < allEvents.Length; i++)
			{
				var eventorEvent = BuildEvent(i, allEvents[i]);
				if (eventorEvent == null)
				{
					continue;
				}

				events.Add(eventorEvent);
			}
			return events.OrderBy(x => x.Distance).ToList();
		}

		EventorEvent BuildEvent(int i, string src)
		{
			if (string.IsNullOrWhiteSpace(src))
			{
				return null;
			}

			var parts = src.Split(',', StringSplitOptions.RemoveEmptyEntries);
			if (parts.Length != 5)
			{
				throw new Exception($"Eventor file, line {i}, malformed");
			}

			var eventorEvent = new EventorEvent();
			var eventDate = new DateTime(_thisYear, int.Parse(parts[1]), int.Parse(parts[2]));
			var yDay = eventDate.DayOfYear;
			var lookAhead = _vpsAppsRc.EventsLookAhead < 1 ? 7 : _vpsAppsRc.EventsLookAhead;
			if (yDay < _thisYday)
			{
				if ((_thisYday + lookAhead) < yDay + _days)
				{
					return null;
				}

				eventorEvent.Distance = yDay + _days - _thisYday;
			}
			else
			{
				if (_thisYday + lookAhead < yDay)
				{
					return null;
				}

				eventorEvent.Distance = yDay - _thisYday;
			}

			var age = _thisYear - int.Parse(parts[0]);
			var text = parts[4];
			if ("f".Equals(parts[3], StringComparison.CurrentCultureIgnoreCase))
			{
				eventorEvent.Glyph = "&#127874;";
				eventorEvent.Text = $"F&oslash;dselsdag: {text} ({age} &aring;r)";
			}
			else if ("d".Equals(parts[3], StringComparison.CurrentCultureIgnoreCase))
			{
				eventorEvent.Glyph = "&#9760;";
				eventorEvent.Text = $"D&oslash;dsdag: {text} ({age} &aring;r siden)";
			}

			if (eventorEvent.Distance == 0)
			{
				eventorEvent.Text = $"<b>{eventorEvent.Text} i dag!</b>";
			}
			else
			{
				var plural = eventorEvent.Distance == 1 ? "" : "e";
				eventorEvent.Text = $"{eventorEvent.Text} om {eventorEvent.Distance} dag{plural}";
			}

			return eventorEvent;
		}
	}
}