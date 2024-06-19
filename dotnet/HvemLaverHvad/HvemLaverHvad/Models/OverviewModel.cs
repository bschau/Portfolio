using HvemLaverHvad.TargetProcess;
using System.Collections.Generic;

namespace HvemLaverHvad.Models
{
    public class OverviewModel
    {
        public OverviewModel()
        {
            Users = new List<OverviewUserModel>();
        }

        public List<OverviewUserModel> Users { get; set; }
    }
}
