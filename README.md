# Frustum Culling 2D
Small project for test frustum culling with sfml for graphics lib.

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/preview.png)

## Description
The frustum culling is simply render only objects in the frustum view.

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/frustum.png)

## How to do ?

### Frustum important data
In the frustum class, there is some constants to see.

```c++
struct Frustum
{
	const float Far = 50.0f;
	const float Fov = glm::pi<float>() / 180.0f * 70.0f;
	const float Fov2 = Fov / 2.0f;
	const float Side = glm::tan(Fov2) * Far;
}
```
Here a scheme for explain the calcul from constant "Side".

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/constants.png)

For refresh here also a scheme for function "tan(x)"

![alt tag](https://raw.githubusercontent.com/firepolo/FrustumCulling2D/master/tan.png)