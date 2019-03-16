class unevenPhysics {
public:
	glm::vec3 worldGravity;

	unevenPhysics(float gravity = -9.81) {
		worldGravity = glm::vec3(0, gravity, 0);
	}

	void loadCollisionMesh(const char* filename) {
		mesh tempMesh(filename);
		tempMesh.swapVertex(meshVertices);
		tempMesh.terminator();
	}

	void terminate() {
		std::vector<vertex>().swap(meshVertices);
	}
private:
	std::vector<vertex> meshVertices;
};