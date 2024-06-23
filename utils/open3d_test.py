import open3d as o3d
import numpy as np

# Create a random point cloud
num_points = 10000
points = np.random.rand(num_points, 3)

# Create an Open3D point cloud
point_cloud = o3d.geometry.PointCloud()
point_cloud.points = o3d.utility.Vector3dVector(points)

# Create visualizer
visualizer = o3d.visualization.Visualizer()
visualizer.create_window()

# Add point cloud to visualizer
visualizer.add_geometry(point_cloud)

# Get the camera parameters
params = visualizer.get_view_control().convert_to_pinhole_camera_parameters()

# Set initial view
params.extrinsic = np.array([[1, 0, 0, 0],
                              [0, 1, 0, 0],
                              [0, 0, 1, 0],
                              [0, 0, 0, 1]])
visualizer.get_view_control().convert_from_pinhole_camera_parameters(params)

# Run visualizer
visualizer.run()

# Pan camera around
for angle in range(0, 360, 5):  # Pan 360 degrees in steps of 5 degrees
    # Set rotation around Y axis
    rotation = np.array([[np.cos(np.radians(angle)), 0, np.sin(np.radians(angle)), 0],
                         [0, 1, 0, 0],
                         [-np.sin(np.radians(angle)), 0, np.cos(np.radians(angle)), 0],
                         [0, 0, 0, 1]])

    # Update camera extrinsic matrix
    params.extrinsic = rotation
    visualizer.get_view_control().convert_from_pinhole_camera_parameters(params)
    visualizer.update_geometry(point_cloud)
    visualizer.poll_events()
    visualizer.update_renderer()

# Close visualizer
visualizer.destroy_window()