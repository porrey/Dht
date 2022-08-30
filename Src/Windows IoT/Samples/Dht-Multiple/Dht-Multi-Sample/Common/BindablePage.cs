// Copyright © 2015-2022 Daniel Porrey
//
// This file is part of the Dht-Multi-Sample Solution.
// 
// Dht-Multi-Sample Solution is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// Dht-Multi-Sample Solution is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with Dht-Multi-Sample Solution. If not, see http://www.gnu.org/licenses/.
//
using System.Collections.Generic;
using System.ComponentModel;
using System.Runtime.CompilerServices;
using Windows.UI.Xaml.Controls;

namespace Dht.Multi.Sample.Common
{
	public abstract class BindablePage : Page, INotifyPropertyChanged
	{
		public event PropertyChangedEventHandler PropertyChanged;

		protected virtual bool SetProperty<T>(ref T storage, T value, [CallerMemberName] string propertyName = null)
		{
			if (EqualityComparer<T>.Default.Equals(storage, value)) return false;

			storage = value;
			this.RaisePropertyChanged(propertyName);

			return true;
		}

		protected void RaisePropertyChanged([CallerMemberName]string propertyName = null)
		{
			this.OnPropertyChanged(propertyName);
		}

		protected virtual void OnPropertyChanged([CallerMemberName]string propertyName = null)
		{
			OnPropertyChanged(new PropertyChangedEventArgs(propertyName));
		}

		protected virtual void OnPropertyChanged(PropertyChangedEventArgs args)
		{
			this.PropertyChanged?.Invoke(this, args);
		}
	}
}
