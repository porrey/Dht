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
using System;
using Windows.UI.Xaml.Data;

namespace Dht.Multi.Sample.Common
{
	public sealed class DoubleToStringConverter : IValueConverter
	{
		public object Convert(object value, Type targetType, object parameter, string language)
		{
			return $"{value:0.0}";
		}

		public object ConvertBack(object value, Type targetType, object parameter, string language)
		{
			return System.Convert.ToDouble(value);
		}
	}
}