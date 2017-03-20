#include "physicssimulation.h"

#include "../common/macro/debuglog.h"

PhysicsSimulation::PhysicsSimulation()
{
    DEBUG_LOG("Creating physics simulation")

    //collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    mCollisionConfiguration = new btDefaultCollisionConfiguration();

    //use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    mCollisionDispatcher = new btCollisionDispatcher(mCollisionConfiguration);

    //btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    mOverlappingPairCache = new btDbvtBroadphase();
    //mOverlappingPairCache = new btAxisSweep3(btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000));

    //the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    mSolver = new btSequentialImpulseConstraintSolver;

    // Initialize the world
    mDynamicsWorld = new btDiscreteDynamicsWorld(mCollisionDispatcher, mOverlappingPairCache, mSolver, mCollisionConfiguration);

    // Set gravity (have to be a little bit accurate :)
    mDynamicsWorld->setGravity(btVector3(0.0f,-9.81f, 0.0f));

    // ghost collision callback
    mGhostPairCallback = new btGhostPairCallback(); // needed for hit-testing "non-physics" shapes
    mDynamicsWorld->getBroadphase()->getOverlappingPairCache()->setInternalGhostPairCallback(mGhostPairCallback); // why?

    // create debug drawer
    // debugDraw = new DebugDrawer;

    // attach debug draw
    // dynamicsWorld->setDebugDrawer(debugDraw);
}


/*
// void PhysicsWorld::addPhysicsObject(RigidBody::Collision shape, par1=0, par2=0, par3=0, par4=0)
void PhysicsSubsystem::addPhysicsDynamic(RigidBody* rigidbody, btCollisionShape* shape)
{
    //create a dynamic rigidbody

    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    // btCollisionShape* colShape = new btSphereShape(btScalar(1.));
    btCollisionShape* colShape = shape;
    collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    btScalar	mass(10.f);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass,localInertia);

    // Set starting rotation
    btQuaternion bt_start_quat = btQuaternion(rigidbody->rot.w, rigidbody->rot.x,
                                              rigidbody->rot.y, rigidbody->rot.z);
    startTransform.setRotation( bt_start_quat ) ;

    // Set starting translation
    startTransform.setOrigin(btVector3(rigidbody->pos.x, rigidbody->pos.y, rigidbody->pos.z));

    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
    rbInfo.m_restitution = btScalar(0.33f);
    rbInfo.m_friction = btScalar(0.4f);
    btRigidBody* body = new btRigidBody(rbInfo);

    rigidbody->setBody(body);

    dynamicsWorld->addRigidBody(body);
}

void PhysicsWorld::addPhysicsStatic(RigidBody* rigidbody, btCollisionShape* shape)
{
    //create a dynamic rigidbody

    //btCollisionShape* colShape = new btBoxShape(btVector3(1,1,1));
    // I think this puts the plane at -1 y?
    btCollisionShape* colShape = shape;
    shape->setLocalScaling(btVector3(rigidbody->scale.x, rigidbody->scale.y, rigidbody->scale.z) );
    collisionShapes.push_back(colShape);

    // Create Dynamic Objects
    btTransform startTransform;
    startTransform.setIdentity();

    // Set scale


    btScalar	mass(0.f);

    //rigidbody is dynamic if and only if mass is non zero, otherwise static
    bool isDynamic = (mass != 0.f);

    btVector3 localInertia(0,0,0);
    if (isDynamic)
        colShape->calculateLocalInertia(mass,localInertia);

   // Set starting rotation
    btQuaternion bt_start_quat = btQuaternion(rigidbody->rot.w, rigidbody->rot.x,
                                              rigidbody->rot.y, rigidbody->rot.z);
    startTransform.setRotation( bt_start_quat ) ;

    // Set starting translation
    startTransform.setOrigin(btVector3(rigidbody->pos.x, rigidbody->pos.y, rigidbody->pos.z));



    //using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass,myMotionState,colShape,localInertia);
//    rbInfo.m_restitution = btScalar(0.99f);
//    rbInfo.m_friction = btScalar(0.99f);
    rbInfo.m_restitution = btScalar(0.33f);
    rbInfo.m_friction = btScalar(0.4f);
    btRigidBody* body = new btRigidBody(rbInfo);

    rigidbody->setBody(body);

    dynamicsWorld->addRigidBody(body);
}
*/

PhysicsSimulation::~PhysicsSimulation()
{
    for (int i=mDynamicsWorld->getNumCollisionObjects()-1; i>=0 ;i--)
    {
        btCollisionObject* obj = mDynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        mDynamicsWorld->removeCollisionObject( obj );
        delete obj;
    }

    //delete collision shapes
    for (int j=0;j<mCollisionShapes.size();j++)
    {
        btCollisionShape* shape = mCollisionShapes[j];
        mCollisionShapes[j] = 0;
        delete shape;
    }

    //delete debugDraw;

    delete mGhostPairCallback;

    delete mDynamicsWorld;

    delete mSolver;

    delete mOverlappingPairCache;

    delete mCollisionDispatcher;

    delete mCollisionConfiguration;

    mCollisionShapes.clear();
}
