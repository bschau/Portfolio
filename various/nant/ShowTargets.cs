using System;
using System.Collections.Generic;
using System.Linq;
using NAnt.Core;
using NAnt.Core.Attributes;

namespace Nant
{
	[TaskName("showtargets")]
	internal class ShowTargets : Task
	{
		protected override void ExecuteTask()
		{
			var targets = Project.Targets.Cast<Target>().OrderBy(t => t.Name).ToList(); /* Where(t => !string.IsNullOrEmpty(t.Description)).*/
			var mainTargets = targets.Where(t => !string.IsNullOrEmpty(t.Description)).ToList();
			var subTargets = targets.Where(t => string.IsNullOrEmpty(t.Description)).ToList();
			var defaultTarget = GetDefaultTarget();
			var targetFormat = GetFormatString(mainTargets, subTargets, defaultTarget);

			ShowDefaultTarget(defaultTarget, targetFormat);
			ShowDividerLine(mainTargets.Any() || subTargets.Any());
			ShowTargetsList(mainTargets, targetFormat);
			ShowDividerLine(subTargets.Any());
			ShowSubTargetsList(subTargets);
			ShowDividerLine();
		}

		private Target GetDefaultTarget()
		{
			if (Project.DefaultTargetName == null)
			{
				return null;
			}

			return Project.Targets.Find(Project.DefaultTargetName);
		}

		private string GetFormatString(List<Target> mainTargets, List<Target> subTargets, Target defaultTarget)
		{
			var mainTargetsLength = mainTargets.Select(x => x.Name.Length).Max();
			var length = defaultTarget == null ? 0 : defaultTarget.Name.Length;

			if (mainTargetsLength > length)
			{
				length = mainTargetsLength;
			}

			return string.Format("{{0,{0}}}   {{1}}", length);
		}

		private void ShowDefaultTarget(Target target, string targetFormat)
		{
			Console.WriteLine("Default target: ");
			if (target == null)
			{
				return;
			}

			Console.WriteLine(targetFormat, target.Name, target.Description);
		}

		private void ShowDividerLine(bool showDividerLine = true)
		{
			if (!showDividerLine)
			{
				return;
			}

			Console.WriteLine();
		}

		private void ShowTargetsList(List<Target> targets, string targetFormat)
		{
			Console.WriteLine("Main targets:");
			if (!targets.Any())
			{
				return;
			}

			foreach (var target in targets)
			{
				Console.WriteLine(targetFormat, target.Name, target.Description);
			}
		}

		private void ShowSubTargetsList(List<Target> targets)
		{
			Console.WriteLine("Sub-targets:");
			if (!targets.Any())
			{
				return;
			}

			Console.Write("    ");
			var position = 4;
			foreach (var target in targets)
			{
				var nameLen = target.Name.Length + 2;
				if ((position + nameLen) > Console.WindowWidth)
				{
					Console.WriteLine();
					Console.Write("    {0}  ", target.Name);
					position = 6 + target.Name.Length;
				}
				else
				{
					Console.Write("{0}  ", target.Name);
					position += target.Name.Length + 2;
				}
			}

			Console.WriteLine();
		}
	}
}
