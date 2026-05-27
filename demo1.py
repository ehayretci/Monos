import requests
import time
import webbrowser

# Get your Google API key
google_api_key = 'YOUR_GOOGLE_MAPS_API_KEY'

# Get the origin address from the user
origin_address = input('Enter your origin address: ')

# Get the destination address from the user
destination_address = input('Enter your destination address: ')

# Use the Google Geocoding API to get the coordinates of an address
geocoding_url = 'https://maps.googleapis.com/maps/api/geocode/json'

def get_coordinates(address):
    geocoding_params = {
        'address': address,
        'key': google_api_key
    }
    geocoding_response = requests.get(geocoding_url, params=geocoding_params)
    geocoding_data = geocoding_response.json()
    if geocoding_data['status'] == 'OK':
        location = geocoding_data['results'][0]['geometry']['location']
        lat = location['lat']
        lng = location['lng']
        print(f'Coordinates for "{address}": {lat}, {lng}')
        return lat, lng
    else:
        print(f'Error geocoding address "{address}":', geocoding_data.get('error_message', geocoding_data['status']))
        exit()

origin_lat, origin_lng = get_coordinates(origin_address)
dest_lat, dest_lng = get_coordinates(destination_address)

directions_url = 'https://maps.googleapis.com/maps/api/directions/json'
directions_params = {
    'origin': f'{origin_lat},{origin_lng}',
    'destination': f'{dest_lat},{dest_lng}',
    'mode': 'bicycling',
    'key': google_api_key
}

directions_response = requests.get(directions_url, params=directions_params)
data = directions_response.json()

if data['status'] != 'OK':
    print('Error fetching data from Google Maps Directions API:', data.get('error_message', data['status']))
    exit()

speed = float(input('Enter your speed in meters per second: '))

maneuvers = []
distances = []

for route in data['routes']:
    for leg in route['legs']:
        for step in leg['steps']:
            maneuver = step.get('maneuver', None)
            maneuvers.append(maneuver)
            distance_value = step['distance']['value']
            distances.append(distance_value)

if not maneuvers:
    print('No maneuvers found in the route. Exiting.')
    exit()

for i in range(len(maneuvers)):
    distance = distances[i]
    wait_time = distance / speed
    print(f'Waiting for {wait_time:.2f} seconds before next maneuver.')
    time.sleep(wait_time)
    maneuver = maneuvers[i]
    if maneuver == 'turn-right':
        webbrowser.open_new_tab('http://<RIGHT_GLOVE_IP>/right')
        print('Turn right signal sent.')
    elif maneuver == 'turn-left':
        webbrowser.open_new_tab('http://<LEFT_GLOVE_IP>/left')
        print('Turn left signal sent.')
    else:
        print(f'Maneuver: {maneuver}. No action required.')

webbrowser.open_new_tab('http://<RIGHT_GLOVE_IP>/arrive')
webbrowser.open_new_tab('http://<LEFT_GLOVE_IP>/arrive')
print('Arrived at destination. Arrival signal sent.')