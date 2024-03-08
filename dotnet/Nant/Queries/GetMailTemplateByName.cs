using System;
using System.Linq;
using System.Reflection;
using Nant.MailTemplates;

namespace Nant.Queries
{
	public class GetMailTemplateByName
	{
		public IMailTemplate Execute(string name)
		{
			var type = typeof (IMailTemplate);
			var types = Assembly.GetExecutingAssembly()
				.GetTypes()
				.Where(x => type.IsAssignableFrom(x)
							&& x.Name != "IMailTemplate");

			var templateName = name.Trim().ToLower();
			var templates = types.Where(x => x.Name.ToLower() == templateName).ToList();
			if (templates.Count == 0)
			{
				throw new ArgumentException(name + ": mail template not found");
			}

			if (templates.Count > 1)
			{
				throw new ArgumentException(name + ": multiple templates found");
			}

			var mailTemplate = Activator.CreateInstance(templates[0]) as IMailTemplate;
			if (mailTemplate == null)
			{
				throw new Exception(name + ": failed to create new instance");
			}

			return mailTemplate;
		}
	}
}
