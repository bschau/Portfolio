namespace Nant.MailTemplates
{
	public class NewReleasePushed : IMailTemplate
	{
		public string Subject
		{
			get
			{
				return "A new release of @SOLUTION@ have been pushed to Github";
			}
		}

		public string Body
		{
			get
			{
				return @"
<p>A new release of @SOLUTION@ has been pushed to GitHub. Please update your local repository:</p>
<p><br /></p>
<pre style=""background-color: #ddd; border: 1px dotted black;"">
	cd /c/work/@SOLUTION@
	nant update

</pre>
<p>... or ...</p>
<pre style=""background-color: #ddd; border: 1px dotted black;"">
	cd /c/work/@SOLUTION@
	git pull real-master master
	git push origin master

</pre>
<p><br /></p>
<p>Thanks, and enjoy!</p>
";
			}
		}
	}
}
