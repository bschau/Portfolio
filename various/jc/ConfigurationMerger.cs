using System;
using System.Collections.Generic;
using System.Linq;
using jc.DomainModel;
using Newtonsoft.Json;
using Newtonsoft.Json.Linq;

namespace jc
{
	class ConfigurationMerger
	{
		readonly Configuration _configuration;
		bool _profileSeen;
		JProperty _profileProperty;

		internal ConfigurationMerger(Configuration configuration)
		{
			_configuration = configuration;
		}

		internal void Merge(string rcFile)
		{
			var jSon = JObject.Parse(rcFile);
			ParseAndMerge(jSon, _configuration.Profile);
			ParseProfiles(_profileProperty, _configuration.Profile);
			EnsureValidProfile(_configuration.Profile);
		}

		void ParseAndMerge(JObject jSon, string profile)
		{
			var variables = _configuration.Variables;
			foreach (var node in jSon.Children().Where(x => x.Type == JTokenType.Property))
			{
				var property = node.Value<JProperty>();
				if ("headers".Equals(property.Name, StringComparison.CurrentCultureIgnoreCase))
				{
					ParseHeaders(property);
					continue;
				}

				if ("profiles".Equals(node.Path, StringComparison.CurrentCultureIgnoreCase))
				{
					if (profile == null)
					{
						throw new ArgumentException("You cannot have Profiles in Profiles");
					}

					_profileProperty = property;
					continue;
				}

				AddKeyValue(variables, property.Name.ToLower(), property.Value.Value<string>());
			}
		}

		void AddKeyValue(Dictionary<string, string> dictionary, string key, string value)
		{
			var testKey = dictionary.Keys.SingleOrDefault(x => key.Equals(x, StringComparison.CurrentCultureIgnoreCase));
			if (testKey == null)
			{
				dictionary.Add(key, value);
				return;
			}

			dictionary[testKey] = value;
		}

		void ParseHeaders(JProperty property)
		{
			var children = property.Children().Children().ToList();
			var headers = _configuration.Headers;
			for (var index = 0; index < children.Count; index++)
			{
				var child = children[index];
				if (child.Count() != 2)
				{
					throw new ArgumentException("Malformed Headers section");
				}
				AddKeyValue(headers, child[0].Value<string>(), child[1].Value<string>());
			}
		}

		void ParseProfiles(JProperty property, string profile)
		{
			var profiles = property.Children().ToList();
			if (profiles[0].Type != JTokenType.Array)
			{
				throw new ArgumentException("The Profiles property must contain an array of profiles");
			}
			
			for (var index = 0; index < profiles.Count(); index++)
			{
				var profileObject = profiles[index];
				if (MergeProfile(profileObject, profile))
				{
					return;
				}
			}
		}

		bool MergeProfile(JToken profileToken, string profile)
		{
			if (profileToken.Type != JTokenType.Array)
			{
				throw new ArgumentException("The Profiles property must contain an array of profiles");
			}

			for (var index = 0; index < profileToken.Count(); index++)
			{
				if (MergeThisProfile(profileToken[index], profile))
				{
					return true;
				}
			}

			return false;
		}

		bool MergeThisProfile(JToken profileObject, string profile)
		{
			var children = profileObject.Children().ToList();
			for (var index = 0; index < children.Count(); index++)
			{
				var pair = children[index] as JProperty;
				if (!"name".Equals(pair.Name, StringComparison.CurrentCultureIgnoreCase))
				{
					continue;
				}

				var value = pair.First().Value<string>();
				if (profile.Equals(value, StringComparison.CurrentCultureIgnoreCase))
				{
					var json = JsonConvert.SerializeObject(profileObject);
					ParseAndMerge(JObject.Parse(json), profile: null);
					_profileSeen = true;
					return true;
				}
			}

			return false;
		}

		void EnsureValidProfile(string profile)
		{
			if (!_profileSeen)
			{
				throw new ArgumentException($"Profile '{profile}' not found");
			}
		}
	}
}