import serial
import numpy as np
import open3d as o3d

import argparse

import time



def main(args):
    if args.load is None:
        data = gather_data(args.save)
    else:
        data = np.loadtxt(args.load)

    render_data(data)

    

def get_calibration_data(data):
    x_min = np.min(data[...,0])
    x_max = np.max(data[...,0])
    y_min = np.min(data[...,1])
    y_max = np.max(data[...,1])
    z_min = np.min(data[...,2])
    z_max = np.max(data[...,2])

    print("xmin/xmax = ", x_min, "/" , x_max, "ymin/ymax = " , y_min, "/", y_max,  "zmin/zmax = " , z_min, "/", z_max)

    x_calib = (x_max + x_min )/2
    y_calib = (y_max + y_min )/2
    z_calib = (z_max + z_min )/2

    print("x_calib", x_calib, "y_calib", y_calib, "z_calib", z_calib)

    calib_data = np.copy(data)
    calib_data = calib_data - [x_calib, y_calib, z_calib]

    return calib_data


def render_data(data):
    print("visualizing data ", data.shape)
    import open3d as o3d
    import numpy as np


    
    points = data[0:1000]


    calib_data = get_calibration_data(points)
    # points = calib_data


    colors = np.zeros_like(points)
    colors[0] = (1.0,0,0)
    # Create an Open3D point cloud
    point_cloud = o3d.geometry.PointCloud()
    point_cloud.points = o3d.utility.Vector3dVector(points)
    point_cloud.colors = o3d.utility.Vector3dVector(colors)

    width=800
    height=600
    # Create visualizer
    visualizer = o3d.visualization.Visualizer()
    visualizer.create_window(width=width, height=height)

    axes = o3d.geometry.TriangleMesh.create_coordinate_frame(size=20.0, origin=[0, 0, 0])

    # Add point cloud to visualizer
    visualizer.add_geometry(point_cloud)
    visualizer.add_geometry(axes)

    # Get the camera parameters
    params = visualizer.get_view_control().convert_to_pinhole_camera_parameters()

    # Set initial view
    params.extrinsic = np.array([[1, 0, 0, 0],
                                [0, 1, 0, 0],
                                [0, 0, 1, 0],
                                [0, 0, 0, 1]])
    visualizer.get_view_control().convert_from_pinhole_camera_parameters(params)


    while True:
        visualizer.update_geometry(point_cloud)
        visualizer.poll_events()
        visualizer.update_renderer()
        if visualizer.poll_events() == False:
            break

    visualizer.destroy_window()


def gather_data(save_name):
    ser = serial.Serial(port='COM8', baudrate=9600)

    max_data = 10000
    data = np.zeros((max_data,3))

    data_idx = 1

    try:
        while True:
            try:
                d = ser.readline().decode().strip()
                print(d)
                x, y, z = map(float, d.split(','))
                data[data_idx] = [x,y,z]
                data_idx+=1
                if data_idx >= max_data:
                    break
            except ValueError:
                pass
    except KeyboardInterrupt:
        print("Program terminated by user.")
        ser.close()

    finally:
        if save_name is None:
            save_name = f"magnet_data_{int(time.time())}.clb"
        
        np.savetxt(save_name, data)

    print(f"saving {data_idx} data points")





if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-l", "--load", type=str)
    parser.add_argument("-s", "--save", type=str)
    args = parser.parse_args()
    print(args)
    main(args)

