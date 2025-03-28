from venv import create
from reportlab.pdfgen import canvas

# Conversion factors
MM_TO_POINTS = 2.8346456693


def create(filename, paper_size, sensor_sizes, center, radius_increment, num_circles):
    c = canvas.Canvas(filename, pagesize=(paper_size[0] * MM_TO_POINTS, paper_size[1] * MM_TO_POINTS))

    # Draw circles and labels
    for i in range(1, num_circles + 1):
        radius = i * radius_increment
        c.circle(center[0] * MM_TO_POINTS, center[1] * MM_TO_POINTS, radius * MM_TO_POINTS)

        # Add label (distance in mm)
        c.setFont("Helvetica", 10)
        c.drawString(center[0] * MM_TO_POINTS, center[1] * MM_TO_POINTS + radius * MM_TO_POINTS + 5, f"{radius} mm")

    # Draw a 5 cm calibration line
    calibration_length_pts = 50 * MM_TO_POINTS  # 50 mm in points
    calibration_x_start = 50  # 50 points from the left edge
    calibration_y = 100  # Position near the bottom

    c.line(calibration_x_start, calibration_y, calibration_x_start + calibration_length_pts, calibration_y)
    c.drawString(calibration_x_start, calibration_y + 5, "5 cm")

    for sensor_size in sensor_sizes:
        sensor_x = center[0] - (sensor_size[0] / 2)
        sensor_y = center[1] - (sensor_size[1] / 2)
        # Draw sensor rectangle at the center
        c.rect(sensor_x * MM_TO_POINTS, sensor_y * MM_TO_POINTS, sensor_size[0] * MM_TO_POINTS,
               sensor_size[1] * MM_TO_POINTS)

        # Label the sensor dimensions
        c.drawString(sensor_x * MM_TO_POINTS, sensor_y * MM_TO_POINTS - 10, f"{sensor_size[0]}mm x {sensor_size[1]}mm")

    # Save the PDF
    c.showPage()
    c.save()

    print(f"PDF saved as {filename}")


if __name__ == '__main__':
    # Define A3 paper size
    paper_size = (297, 420)
    sensor_size = (25, 18)
    sensor_size2 = (20, 8)
    center_position = (paper_size[0] / 2, paper_size[1] / 2)
    center_position2 = (paper_size[0] / 2, 30)
    create("circles_a3.pdf", paper_size, [sensor_size, sensor_size2], center_position, 30, 20)
    create("halfcircles_a3.pdf", paper_size, [sensor_size, sensor_size2], center_position2, 30, 20)
