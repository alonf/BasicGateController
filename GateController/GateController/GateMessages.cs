using System;
using System.Collections.ObjectModel;

namespace GateController
{
    public class GateMessages
    {
        public ObservableCollection<string> Messages { get; } = new ObservableCollection<String>();
    }
}
