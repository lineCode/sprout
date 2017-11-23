#ifndef SU_CORE_SCENE_ANIMATION_STAGE_HPP
#define SU_CORE_SCENE_ANIMATION_STAGE_HPP

namespace scene {

namespace entity { class Entity; }

namespace animation {

class Animation;

class Stage {

public:

    Stage(entity::Entity* entity, Animation* animation);

	void update();

private:

    entity::Entity* entity_;
    Animation*   animation_;
};

}}

#endif
