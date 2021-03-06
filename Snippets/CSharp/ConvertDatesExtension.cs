using System;
using System.Text.RegularExpressions;

namespace AddoSamples.Extensions
{
    // Convert from Microsoft JSON date to ISO 8601 date.
    // Sometimes when you get JSON back from a service it will have dates in Microsoft JSON date format.
    // Ironically, when parsing this with Microsoft.Json it fails.
    //
    // Usage:
    //
    //    var unconvertedJson = ... get from service ...
    //    var convertedJson = unconvertedJson.ConvertDates();
    //    var targetObject = JsonSerializer.Deserialize<TargetObject>(convertedJson);
    //
    public static class StringExtensions
    {
        static readonly Regex _convertDates = new Regex(@"(\\\/Date\()(\d+)(\)\\\/)", RegexOptions.Compiled | RegexOptions.IgnoreCase);
        static readonly DateTime _origo = new DateTime(1970, 1, 1, 0, 0, 0, 0, DateTimeKind.Utc);

        public static string ConvertDates(this string src)
        {
            if (string.IsNullOrWhiteSpace(src))
            {
                return string.Empty;
            }

            return _convertDates.Replace(src, new MatchEvaluator(MillisToDate));
        }

        static string MillisToDate(Match match)
        {
            var matchValue = long.Parse(match.Groups[2].ToString());
            var dateTime = _origo.AddMilliseconds(matchValue);
            return dateTime.ToString("yyyy-MM-ddThh:mm:ssZ");
        }
    }
}