import unittest
import requests
import http.client


class TestServerResponse(unittest.TestCase):
    def test_get(self):
        url = "http://localhost:8081"

        response = requests.get(
            url=url
        )
        self.assertEqual(response.status_code, 200)

    def test_get_not_found(self):
        url = "http://localhost:8081/doesntexist"

        response = requests.get(
            url=url
        )
        self.assertEqual(response.status_code, 404)

    def test_get_method_not_allowed(self):
        url = "http://localhost:8081/no-get"

        response = requests.get(
            url=url
        )
        self.assertEqual(response.status_code, 405)

    def test_get_autoindex(self):
        url = "http://localhost:8081/autoindex/"

        response = requests.get(
            url=url
        )
        self.assertEqual(response.status_code, 200)
	
	# def test_get_redirection

    def test_post(self):
        url = "http://localhost:8081/upload"
        files = {'file': open('tests/Poster.jpg', 'rb')}
        headers = {
            'Content-Length': '1046495',
        }

        response = requests.post(
            url=url,
            headers=headers,
            files=files,
        )
        files["file"].close()
        self.assertTrue(response.history)
        self.assertEqual(response.history[0].status_code, 303)

	# NB I struggled to make a simple post request so I copied yours
    def test_post_bigger_max_client_size(self):
        url = "http://localhost:8081/post-bigger"
        files = {'file': open('tests/Poster.jpg', 'rb')}
        headers = {
            'Content-Length': '1046495',
        }

        response = requests.post(
            url=url,
            headers=headers,
            files=files,
        )
        files["file"].close()
        self.assertEqual(response.status_code, 413)

    def test_post_method_not_allowed(self):
        url = "http://localhost:8081/no-post"
        files = {'file': open('tests/Poster.jpg', 'rb')}
        headers = {
            'Content-Length': '1046495',
        }

        response = requests.post(
            url=url,
            headers=headers,
            files=files,
        )
        files["file"].close()
        self.assertEqual(response.status_code, 405)

    def test_post_chunked(self):
        conn = http.client.HTTPConnection("localhost:8081")

        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        # Write the chunked data
        conn.send(b"7\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")

        # Get the response
        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 200)
        # print(response.status, response.reason)
        # print(response.read())

	# This should be 400 Bad Request
    def test_post_invalid_chunks(self):
        conn = http.client.HTTPConnection("localhost:8081")

        conn.putrequest("POST", "/upload")
        conn.putheader("Transfer-Encoding", "chunked")
        conn.endheaders()

        conn.send(b"9\r\nMozilla\r\n11\r\nDeveloper Network\r\n0\r\n\r\n")

        response = conn.getresponse()
        conn.close()
        self.assertEqual(response.status, 500)

	# We check if the file exists and return an error if not
    def test_delete(self):
        filename = 'test.txt'
        file_path = f"./www/delete/{filename}"

        # Create the file
        with open(file_path, 'w') as file:
            file.write("Test content")

        url = f"http://localhost:8081/delete/{filename}"
        response = requests.delete(
            url=url
        )
        self.assertEqual(response.status_code, 200)

    def test_delete_method_not_allowed(self):
        url = "http://localhost:8081/no-delete/test"
        response = requests.delete(
            url=url
        )
        self.assertEqual(response.status_code, 405)

	# Should it be a 400 Bad Request ?
    def test_delete_file_does_not_exist(self):
        url = "http://localhost:8081/delete/does_not_exist"
        response = requests.delete(
            url=url
        )
        self.assertEqual(response.status_code, 500)

	# def test_unknown_requet


if __name__ == '__main__':
    unittest.main()
