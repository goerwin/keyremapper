//
//  Global.swift
//  Daemon
//
//  Created by Erwin Gaitan Ospino on 26/03/22.
//

import Foundation

struct Global {
    static let VERSION = Bundle.main.infoDictionary?["CFBundleShortVersionString"] as! String
    static var appBridge: AppBridge?
}


