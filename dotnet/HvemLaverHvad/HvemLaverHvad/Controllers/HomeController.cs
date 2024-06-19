using System.Web.Mvc;

namespace HvemLaverHvad.Controllers
{
	public class HomeController : Controller
	{
		public ActionResult Index()
		{
			return RedirectPermanent("/Overview");
		}
	}
}
