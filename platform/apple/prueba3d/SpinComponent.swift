//
//  SpinComponent.swift
//  prueba3d
//
//  Created by Francisco Abraham Espinoza Silvas on 08/04/2026.
//

import RealityKit

/// A component that spins the entity around a given axis.
struct SpinComponent: Component {
    let spinAxis: SIMD3<Float> = [0, 1, 0]
}
